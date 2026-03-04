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
                              INCLUDES
--------------------------------------------------------------------*/
#include "wifiAPI.hpp"
#include <cstring>

/*--------------------------------------------------------------------
                          GLOBAL NAMESPACES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
core::wifiInterface::wifiInterface()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::wifiInterface (constructor)
 *
 *   DESCRIPTION:
 *       Constructor for wifiInterface class
 *
 *********************************************************************/
core::wifiInterface::wifiInterface(core::console &c_ref)
    : p_console(c_ref), p_server_pcb(nullptr), p_client_pcb(nullptr),
      p_is_server(false), p_connection_open(false), p_rx_buffer_len(0) {
  memset(p_rx_buffer, 0, sizeof(p_rx_buffer));
} /* wifiInterface::wifiInterface() */

/*--------------------------------------------------------------------
core::wifiInterface::~wifiInterface()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::~wifiInterface (deconstructor)
 *
 *   DESCRIPTION:
 *       Deconstructor for wifiInterface class
 *
 *********************************************************************/
core::wifiInterface::~wifiInterface(void) {
  /*----------------------------------------------------------
  Close connection and deinit arch
  ----------------------------------------------------------*/
  close_connection();
  cyw43_arch_deinit();

} /* wifiInterface::~wifiInterface() */

/*--------------------------------------------------------------------
core::wifiInterface::init()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::init()
 *
 *   DESCRIPTION:
 *       Initializes the wifi interface
 *
 *********************************************************************/
bool core::wifiInterface::init(const char *ssid, const char *password) {
  /*----------------------------------------------------------
  Initialize CYW43 arch
  ----------------------------------------------------------*/
  if (cyw43_arch_init()) {
    // p_console.add_assert("Wi-Fi init failed");
    return false;
  }
  cyw43_arch_enable_sta_mode();

  /*----------------------------------------------------------
  Connect to Wi-Fi
  ----------------------------------------------------------*/
  // p_console.log("Connecting to Wi-Fi...");
  if (cyw43_arch_wifi_connect_timeout_ms(ssid, password,
                                         CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    // p_console.add_assert("failed to connect.");
    return false;
  }
  // p_console.log("Connected.");
  return true;

} /* wifiInterface::init() */

/*--------------------------------------------------------------------
core::wifiInterface::start_server()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::start_server()
 *
 *   DESCRIPTION:
 *       Starts a tcp server
 *
 *********************************************************************/
bool core::wifiInterface::start_server(uint16_t port) {
  /*----------------------------------------------------------
  Create Server PCB
  ----------------------------------------------------------*/
  p_is_server = true;
  p_server_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  if (!p_server_pcb) {
    // p_console.add_assert("failed to create pcb");
    return false;
  }

  /*----------------------------------------------------------
  Bind Server PCB to port
  ----------------------------------------------------------*/
  err_t err = tcp_bind(p_server_pcb, IP_ADDR_ANY, port);
  if (err) {
    // p_console.add_assert("failed to bind pcb");
    return false;
  }

  /*----------------------------------------------------------
  Listen on Server PCB
  ----------------------------------------------------------*/
  p_server_pcb = tcp_listen(p_server_pcb);
  if (!p_server_pcb) {
    // p_console.add_assert("failed to listen");
    return false;
  }

  /*----------------------------------------------------------
  Setup Accept Callback
  ----------------------------------------------------------*/
  tcp_arg(p_server_pcb, this);
  tcp_accept(p_server_pcb, tcp_server_accept);

  // p_console.log("Server started");
  return true;

} /* wifiInterface::start_server() */

/*--------------------------------------------------------------------
core::wifiInterface::connect_to_server()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::connect_to_server()
 *
 *   DESCRIPTION:
 *       Connects to a tcp server
 *
 *********************************************************************/
bool core::wifiInterface::connect_to_server(const char *server_ip,
                                            uint16_t port) {
  /*----------------------------------------------------------
  Parse IP Address
  ----------------------------------------------------------*/
  p_is_server = false;
  ip_addr_t target_addr;
  if (!ip4addr_aton(server_ip, &target_addr)) {
    // p_console.add_assert("failed to parse ip address");
    return false;
  }

  /*----------------------------------------------------------
  Create Client PCB
  ----------------------------------------------------------*/
  p_client_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
  if (!p_client_pcb) {
    // p_console.add_assert("failed to create pcb");
    return false;
  }

  /*----------------------------------------------------------
  Begin Connection
  ----------------------------------------------------------*/
  tcp_arg(p_client_pcb, this);

  cyw43_arch_lwip_begin();
  err_t err =
      tcp_connect(p_client_pcb, &target_addr, port, tcp_client_connected);
  cyw43_arch_lwip_end();

  if (err) {
    // p_console.add_assert("failed to connect");
    return false;
  }

  return true;

} /* wifiInterface::connect_to_server() */

/*--------------------------------------------------------------------
core::wifiInterface::send_message()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::send_message()
 *
 *   DESCRIPTION:
 *       Sends a message over tcp
 *
 *********************************************************************/
bool core::wifiInterface::send_message(const uint8_t *data, uint16_t len) {
  /*----------------------------------------------------------
  Check Connection Status
  ----------------------------------------------------------*/
  if (!p_connection_open) {
    return false;
  }

  struct tcp_pcb *pcb = p_is_server ? p_client_pcb : p_server_pcb;
  if (!pcb) {
    return false;
  }

  /*----------------------------------------------------------
  Write Data
  ----------------------------------------------------------*/
  err_t err = tcp_write(pcb, data, len, TCP_WRITE_FLAG_COPY);
  if (err) {
    // p_console.add_assert("failed to write data");
    return false;
  }

  /*----------------------------------------------------------
  Output Data
  ----------------------------------------------------------*/
  err = tcp_output(pcb);
  if (err) {
    // p_console.add_assert("failed to output data");
    return false;
  }

  return true;

} /* wifiInterface::send_message() */

/*--------------------------------------------------------------------
core::wifiInterface::get_message()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::get_message()
 *
 *   DESCRIPTION:
 *       Gets a message from the tcp buffer
 *
 *********************************************************************/
int32_t core::wifiInterface::get_message(uint8_t *buffer, uint16_t len) {
  /*----------------------------------------------------------
  Copy out rx buffer
  ----------------------------------------------------------*/
  if (p_rx_buffer_len > 0) {
    int32_t read_len = p_rx_buffer_len < len ? p_rx_buffer_len : len;
    memcpy(buffer, p_rx_buffer, read_len);
    p_rx_buffer_len = 0;
    return read_len;
  }
  return 0;

} /* wifiInterface::get_message() */

/*--------------------------------------------------------------------
core::wifiInterface::close_connection()
--------------------------------------------------------------------*/
/*********************************************************************
 *
 *   PROCEDURE NAME:
 *       wifiInterface::close_connection()
 *
 *   DESCRIPTION:
 *       Closes the tcp connection
 *
 *********************************************************************/
void core::wifiInterface::close_connection(void) {
  /*----------------------------------------------------------
  Close specific PCB
  ----------------------------------------------------------*/
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

} /* wifiInterface::close_connection() */

/*--------------------------------------------------------------------
core::wifiInterface::tcp_server_sent()
--------------------------------------------------------------------*/
err_t core::wifiInterface::tcp_server_sent(void *arg, struct tcp_pcb *tpcb,
                                           u16_t len) {
  return ERR_OK;

} /* wifiInterface::tcp_server_sent() */

/*--------------------------------------------------------------------
core::wifiInterface::tcp_server_recv()
--------------------------------------------------------------------*/
err_t core::wifiInterface::tcp_server_recv(void *arg, struct tcp_pcb *tpcb,
                                           struct pbuf *p, err_t err) {
  wifiInterface *self = (wifiInterface *)arg;
  if (p == NULL) {
    // self->p_console.log("Connection closed");
    self->close_connection();
    return ERR_OK;
  }
  if (p->tot_len > 0) {
    uint16_t copy_len = p->tot_len;
    if (copy_len > (WIFI_RX_BUFFER_SIZE - self->p_rx_buffer_len)) {
      copy_len = WIFI_RX_BUFFER_SIZE - self->p_rx_buffer_len;
    }

    // pbuf_copy_partial copys with offset
    if (copy_len > 0) {
      pbuf_copy_partial(p, self->p_rx_buffer + self->p_rx_buffer_len, copy_len,
                        0);
      self->p_rx_buffer_len += copy_len;
      tcp_recved(tpcb, p->tot_len); // let LWIP know we got it
    }
  }
  pbuf_free(p);
  return ERR_OK;

} /* wifiInterface::tcp_server_recv() */

/*--------------------------------------------------------------------
core::wifiInterface::tcp_server_err()
--------------------------------------------------------------------*/
void core::wifiInterface::tcp_server_err(void *arg, err_t err) {
  wifiInterface *self = (wifiInterface *)arg;
  self->p_console.add_assert("tcp error");

} /* wifiInterface::tcp_server_err() */

/*--------------------------------------------------------------------
core::wifiInterface::tcp_server_accept()
--------------------------------------------------------------------*/
err_t core::wifiInterface::tcp_server_accept(void *arg, struct tcp_pcb *newpcb,
                                             err_t err) {
  wifiInterface *self = (wifiInterface *)arg;
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

} /* wifiInterface::tcp_server_accept() */

/*--------------------------------------------------------------------
core::wifiInterface::tcp_client_connected()
--------------------------------------------------------------------*/
err_t core::wifiInterface::tcp_client_connected(void *arg, struct tcp_pcb *tpcb,
                                                err_t err) {
  wifiInterface *self = (wifiInterface *)arg;
  if (err != ERR_OK) {
    self->p_console.add_assert("connect failed");
    return err;
  }
  self->p_connection_open = true;
  tcp_recv(tpcb, tcp_server_recv);
  tcp_sent(tpcb, tcp_server_sent);
  tcp_err(tpcb, tcp_server_err);
  return ERR_OK;

} /* wifiInterface::tcp_client_connected() */
