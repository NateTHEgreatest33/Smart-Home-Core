/*********************************************************************
*
*   NAME:
*       wifiAPI.cpp
*
*   DESCRIPTION:
*       API for interfacing with wifi on the pi pico W
*
*   Copyright 2024 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "wifiAPI.hpp"
#include <cstring>

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   PROCEDURE NAME:
*       wifiInterface()
*
*   DESCRIPTION:
*       Constructor for wifiInterface class
*
*********************************************************************/
core::wifiInterface::wifiInterface(
    core::console& c_ref
) : p_console(c_ref), p_server_pcb(nullptr), p_client_pcb(nullptr), p_is_server(false), p_connection_open(false), p_rx_buffer(nullptr), p_rx_buffer_len(0) {
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       ~wifiInterface()
*
*   DESCRIPTION:
*       Deconstructor for wifiInterface class
*
*********************************************************************/
core::wifiInterface::~wifiInterface() {
    close_connection();
    cyw43_arch_deinit();
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       init
*
*   DESCRIPTION:
*       Initializes the wifi interface
*
*********************************************************************/
bool core::wifiInterface::init(const char* ssid, const char* password) {
    if (cyw43_arch_init()) {
        p_console.add_assert("Wi-Fi init failed");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    p_console.log("Connecting to Wi-Fi...");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        p_console.add_assert("failed to connect.");
        return false;
    }
    p_console.log("Connected.");
    return true;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       start_server
*
*   DESCRIPTION:
*       Starts a tcp server
*
*********************************************************************/
bool core::wifiInterface::start_server(uint16_t port) {
    p_is_server = true;
    p_server_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!p_server_pcb) {
        p_console.add_assert("failed to create pcb");
        return false;
    }

    err_t err = tcp_bind(p_server_pcb, IP_ADDR_ANY, port);
    if (err) {
        p_console.add_assert("failed to bind pcb");
        return false;
    }

    p_server_pcb = tcp_listen(p_server_pcb);
    if (!p_server_pcb) {
        p_console.add_assert("failed to listen");
        return false;
    }

    tcp_arg(p_server_pcb, this);
    tcp_accept(p_server_pcb, tcp_server_accept);

    p_console.log("Server started");
    return true;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       connect_to_server
*
*   DESCRIPTION:
*       Connects to a tcp server
*
*********************************************************************/
bool core::wifiInterface::connect_to_server(const char* server_ip, uint16_t port) {
    p_is_server = false;
    ip_addr_t target_addr;
    if (!ip4addr_aton(server_ip, &target_addr)) {
        p_console.add_assert("failed to parse ip address");
        return false;
    }

    p_client_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!p_client_pcb) {
        p_console.add_assert("failed to create pcb");
        return false;
    }

    tcp_arg(p_client_pcb, this);

    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(p_client_pcb, &target_addr, port, tcp_client_connected);
    cyw43_arch_lwip_end();

    if (err) {
        p_console.add_assert("failed to connect");
        return false;
    }

    return true;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       send_message
*
*   DESCRIPTION:
*       Sends a message over tcp
*
*********************************************************************/
bool core::wifiInterface::send_message(const uint8_t* data, uint16_t len) {
    if (!p_connection_open) {
        return false;
    }

    struct tcp_pcb *pcb = p_is_server ? p_client_pcb : p_server_pcb;
    if (!pcb) {
        return false;
    }

    err_t err = tcp_write(pcb, data, len, TCP_WRITE_FLAG_COPY);
    if (err) {
        p_console.add_assert("failed to write data");
        return false;
    }

    err = tcp_output(pcb);
    if (err) {
        p_console.add_assert("failed to output data");
        return false;
    }

    return true;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       get_message
*
*   DESCRIPTION:
*       Gets a message from the tcp buffer
*
*********************************************************************/
int32_t core::wifiInterface::get_message(uint8_t* buffer, uint16_t len) {
    if (p_rx_buffer_len > 0) {
        int32_t read_len = p_rx_buffer_len < len ? p_rx_buffer_len : len;
        memcpy(buffer, p_rx_buffer, read_len);
        p_rx_buffer_len = 0;
        return read_len;
    }
    return 0;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       close_connection
*
*   DESCRIPTION:
*       Closes the tcp connection
*
*********************************************************************/
void core::wifiInterface::close_connection() {
    struct tcp_pcb *pcb = p_is_server ? p_client_pcb : p_server_pcb;
    if (pcb) {
        tcp_arg(pcb, NULL);
        tcp_close(pcb);
        p_connection_open = false;
        if (p_is_server) {
            p_client_pcb = nullptr;
        } else {
            p_server_pcb = nullptr;
        }
    }
}


err_t core::wifiInterface::tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    return ERR_OK;
}

err_t core::wifiInterface::tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    wifiInterface* self = (wifiInterface*)arg;
    if (p == NULL) {
        self->p_console.log("Connection closed");
        self->close_connection();
        return ERR_OK;
    }
    if (p->tot_len > 0) {
        self->p_rx_buffer = (uint8_t*)malloc(p->tot_len);
        self->p_rx_buffer_len = pbuf_copy_partial(p, self->p_rx_buffer, p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

void core::wifiInterface::tcp_server_err(void *arg, err_t err) {
    wifiInterface* self = (wifiInterface*)arg;
    self->p_console.add_assert("tcp error");
}

err_t core::wifiInterface::tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    wifiInterface* self = (wifiInterface*)arg;
    if (err != ERR_OK || newpcb == NULL) {
        return ERR_VAL;
    }

    self->p_client_pcb = newpcb;
    self->p_connection_open = true;
    tcp_arg(newpcb, self);
    tcp_recv(newpcb, tcp_server_recv);
    tcp_sent(newpcb, tcp_server_sent);
    tcp_err(newpcb, tcp_server_err);
    return ERR_OK;
}

err_t core::wifiInterface::tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    wifiInterface* self = (wifiInterface*)arg;
    if (err != ERR_OK) {
        self->p_console.add_assert("connect failed");
        return err;
    }
    self->p_connection_open = true;
    tcp_recv(tpcb, tcp_server_recv);
    tcp_sent(tpcb, tcp_server_sent);
    tcp_err(tpcb, tcp_server_err);
    return ERR_OK;
}
