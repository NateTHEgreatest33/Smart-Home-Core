/*********************************************************************
*
*   NAME:
*       wifi.cpp
*
*   DESCRIPTION:
*       API for interfacing with the cyw43 wifi chip
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "wifi.hpp"

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

/*********************************************************************
*
*   PROCEDURE NAME:
*       wifiInterface()
*
*   DESCRIPTION:
*       Constructor for wifiInterface class
*
*********************************************************************/
core::wifiInterface::wifiInterface
    (
    void
    )
{
} /* core::wifiInterface::wifiInterface() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       ~wifiInterface()
*
*   DESCRIPTION:
*       Deconstructor for wifiInterface class
*
*********************************************************************/
core::wifiInterface::~wifiInterface
    (
    void
    )
{
} /* core::wifiInterface::~wifiInterface() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::open_client
*
*   DESCRIPTION:
*       Opens a TCP client connection.
*
*********************************************************************/
bool core::wifiInterface::open_client
    (
    void
    )
{
    TCP_CLIENT_T *state = tcp_client_init();
    if (!state) {
        return false;
    }

    // DEBUG_printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), TCP_PORT);
    state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
    if (!state->tcp_pcb) {
        // DEBUG_printf("failed to create pcb\n");
        return false;
    }

    tcp_arg(state->tcp_pcb, state);
    tcp_poll(state->tcp_pcb, this->poll_callback, POLL_TIME_S * 2);
    tcp_sent(state->tcp_pcb, this->send_callback );
    tcp_recv(state->tcp_pcb, this->recv_callback );
    tcp_err(state->tcp_pcb, this->error_callback );

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect( state->tcp_pcb, &state->remote_addr, TCP_PORT, this->connected_callback );
    cyw43_arch_lwip_end();

    return err == ERR_OK;
} /* core::wifiInterface::open_client() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::close_client
*
*   DESCRIPTION:
*       Closes a TCP client connection.
*
*********************************************************************/
bool core::wifiInterface::close_client
    (
    void
    )
{
    // Implementation for opening a client connection
    return false;
} /* core::wifiInterface::close_client() */

//need to rework this!!
TCP_CLIENT_T* core::wifiInterface::tcp_client_init(void) {
    TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    // The IP address of the TCP server to connect to
    ip4addr_aton(TEST_TCP_SERVER_IP, &state->remote_addr);
    return state;
}

void core::wifiInterface::wifi_runtime()
{
cyw43_arch_poll();
}



err_t core::wifiInterface::poll_callback(void *arg, struct tcp_pcb *tpcb){}
err_t core::wifiInterface::send_callback(void *arg, struct tcp_pcb *tpcb, u16_t len){}
err_t core::wifiInterface::recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){}
void  core::wifiInterface::error_callback(void *arg, err_t err){}
err_t core::wifiInterface::connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err){}

