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
#define TCP_PORT 4242
#define DEBUG_printf printf
#define POLL_TIME_S 10
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
    tcp_client_init();

    // DEBUG_printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), TCP_PORT);
    p_state.tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&(p_state.remote_addr)));

    if (!p_state.tcp_pcb) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    tcp_arg(p_state.tcp_pcb, &p_state); //link p_state as the argumentto pass into all client connections that are associated w/ p_state.tcp_pcb
    tcp_poll(p_state.tcp_pcb, this->poll_callback, POLL_TIME_S * 2);
    tcp_sent(p_state.tcp_pcb, this->send_callback );
    tcp_recv(p_state.tcp_pcb, this->recv_callback );
    tcp_err(p_state.tcp_pcb, this->error_callback );

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();

    err_t err = tcp_connect( p_state.tcp_pcb, &(p_state.remote_addr), TCP_PORT, this->connected_callback );

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
    // TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    err_t err = ERR_OK;

    if (p_state.tcp_pcb != NULL) {

        tcp_arg(p_state.tcp_pcb, NULL);
        tcp_poll(p_state.tcp_pcb, NULL, 0);
        tcp_sent(p_state.tcp_pcb, NULL);
        tcp_recv(p_state.tcp_pcb, NULL);
        tcp_err(p_state.tcp_pcb, NULL);
        err = tcp_close(p_state.tcp_pcb);

        if (err != ERR_OK) 
            {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state.tcp_pcb);
            err = ERR_ABRT;
            }

        p_state.tcp_pcb = NULL;
    }

    p_state.complete = true;

    return (err == ERR_OK);
} /* core::wifiInterface::close_client() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::tcp_client_init
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
void core::wifiInterface::tcp_client_init(void) 
{
memset( &p_state, 0x00, sizeof(TCP_CLIENT_T) );

// The IP address of the TCP server to connect to
ip4addr_aton(TEST_TCP_SERVER_IP, &(p_state.remote_addr));

} /* core::wifiInterface::tcp_client_init() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::wifi_runtime
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
void core::wifiInterface::wifi_runtime()
{
cyw43_arch_poll();
} /* core::wifiInterface::wifi_runtime() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::send_callback
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
err_t core::wifiInterface::send_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
DEBUG_printf("tcp_client_sent %u\n", len);
// We have sent our data, now we can close the connection
DEBUG_printf("All data sent, closing connection\n");
// tcp_result(arg, 0);
return state->error_code;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::recv_callback
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
err_t core::wifiInterface::recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg; //we can access this as state or locally as this->p_state
if (!p) {
    DEBUG_printf("Connection closed by server\n");
    return tcp_result(arg, 0); // 0 for success, server closed connection
}
// this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
// can use this method to cause an assertion in debug mode, if this method is called when
// cyw43_arch_lwip_begin IS needed
cyw43_arch_lwip_check();
if (p->tot_len > 0) {
    DEBUG_printf("Received %d bytes\n", p->tot_len);
    pbuf_copy_partial(p, state->buffer, p->tot_len, 0);
    printf("--- Server response ---\n%s\n-----------------------\n", state->buffer);
    tcp_recved(tpcb, p->tot_len);
}
pbuf_free(p);
return ERR_OK;
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::poll_callback
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
err_t core::wifiInterface::poll_callback(void *arg, struct tcp_pcb *tpcb)
{
DEBUG_printf("tcp_client_poll\n");
return ERR_OK;
}



/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::error_callback
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
void  core::wifiInterface::error_callback(void *arg, err_t err)
{
if (err != ERR_ABRT) 
    DEBUG_printf("tcp_client_err %d\n", err);
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       core::wifiInterface::connected_callback
*
*   DESCRIPTION:
*       x
*
*********************************************************************/
err_t core::wifiInterface::connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err)
{
TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
if (err != ERR_OK) {
    DEBUG_printf("connect failed %d\n", err);
    return tcp_result(arg, err);
}
DEBUG_printf("Connected to server, sending message\n");
const char *message = "Hello from Pico W!";
err = tcp_write(tpcb, message, strlen(message), TCP_WRITE_FLAG_COPY);
if (err != ERR_OK) {
    DEBUG_printf("Failed to write data %d\n", err);
    return tcp_result(arg, -1);
}
return ERR_OK;
}

