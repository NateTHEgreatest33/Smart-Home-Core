/*********************************************************************
*
*   HEADER:
*       header file for wifi (tcp) class. This utilimatley is a wrapper
*       for the cyw43 lib calls
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

#ifndef WIFI_HPP
#define WIFI_HPP

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
// #if !defined(TEST_TCP_SERVER_IP)
//     #error TEST_TCP_SERVER_IP not defined
// #endif
/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    bool connected;
} TCP_CLIENT_T;

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                               CLASSES
--------------------------------------------------------------------*/
namespace core 
{ 

class wifiInterface 
    {
    public:
    wifiInterface();
    ~wifiInterface();

    private:
    bool open_client(void);
    bool close_client(void);

    err_t poll_callback(void *arg, struct tcp_pcb *tpcb);
    err_t send_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
    err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    void  error_callback(void *arg, err_t err);
    err_t connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
    
    };

}

#endif // WIFI_HPP