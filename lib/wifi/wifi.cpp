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
    // Implementation for opening a client connection
    return false;
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




err_t core::wifiInterface::poll_callback(void *arg, struct tcp_pcb *tpcb){}
err_t core::wifiInterface::send_callback(void *arg, struct tcp_pcb *tpcb, u16_t len){}
err_t core::wifiInterface::recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){}
void  core::wifiInterface::error_callback(void *arg, err_t err){}
err_t core::wifiInterface::connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err){}