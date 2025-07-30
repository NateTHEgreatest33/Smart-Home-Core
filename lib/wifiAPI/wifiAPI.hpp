#ifndef WIFI_API_HPP
#define WIFI_API_HPP

/*********************************************************************
*
*   HEADER:
*       header file for wifiAPI
*
*   Copyright 2024 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string>

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "console.hpp"

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/
typedef uint8_t wifi_errors;       /* Error Codes                */
enum 
    {
    WIFI_NO_ERROR,
    WIFI_CONNECT_ERROR,
    WIFI_SEND_ERROR,
    WIFI_RECEIVE_ERROR,
    WIFI_CLOSE_ERROR
    }; 

/*--------------------------------------------------------------------
                               CLASSES
--------------------------------------------------------------------*/
namespace core 
{

class wifiInterface
    {
    public:
        wifiInterface( core::console& c_ref );
        ~wifiInterface();

        bool init(const char* ssid, const char* password);
        bool start_server(uint16_t port);
        bool connect_to_server(const char* server_ip, uint16_t port);
        bool send_message(const uint8_t* data, uint16_t len);
        int32_t get_message(uint8_t* buffer, uint16_t len);
        void close_connection();

    private:
        static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
        static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
        static void tcp_server_err(void *arg, err_t err);
        static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
        static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);


        core::console& p_console;
        struct tcp_pcb *p_server_pcb;
        struct tcp_pcb *p_client_pcb;
        bool p_is_server;
        bool p_connection_open;
        uint8_t* p_rx_buffer;
        uint16_t p_rx_buffer_len;
    };

} /* core namespace */

#endif
