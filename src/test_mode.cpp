
/*********************************************************************
*
*   NAME:
*       test_mode.cpp
*
*   DESCRIPTION:
*       Test mode handles all changes that are needed to adequately
*       test module functionality. This is seperated into a seperate
*       file in order to keep changes isolated from main.cpp
*
*   Copyright 2024 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                              INCLUDES
--------------------------------------------------------------------*/
#include "test_mode.hpp"

extern "C" { 
    #include "messageAPI.h" 
    #include <cstring>
    }

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
                              EXTERNS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                EXTERNS
--------------------------------------------------------------------*/
extern bool g_test_mode_enable;

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_runtime
*
*   DESCRIPTION:
*       handle all test mode changes
*
*********************************************************************/
static void test_mode_runtime
    (
    core::console& c            /* reference to global console      */
    )
{
    
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
rx_message       rx_msg;    /* returned message structure */
tx_message       tx_msg;    /* transmit message structure */
lora_errors      lora_err_var; /* lora errors             */
bool             msgRxed;  /* message rx'ed               */

/*------------------------------------------------------
Check for new messages
------------------------------------------------------*/
msgRxed = get_message( &rx_msg, &lora_err_var );

/*------------------------------------------------------
Message API Testing:
Non distructive testing replies
------------------------------------------------------*/
if(  msgRxed == true && lora_err_var == RX_NO_ERROR )
    {
    memset( &tx_msg, 0, sizeof( tx_message ) );
    tx_msg.destination = RPI_MODULE;

    tx_msg.message[0] = 0xAA;
    tx_msg.size       = 2;
    switch( rx_msg.size )
        {
        case 0x01:
            tx_msg.message[1] = 0x11;
            break;
        case 0x0A:
            tx_msg.message[1] = 0x22;
            break;
        default:
            tx_msg.message[1] = 0xFF;
            break;
        }
    send_message(tx_msg);
    }

/*----------------------------------------------------------
Distructive testing
----------------------------------------------------------*/
if(lora_err_var != RX_NO_ERROR )
    {
    memset( &tx_msg, 0, sizeof( tx_message ) );
    tx_msg.destination = RPI_MODULE;

    tx_msg.message[0] = 0xBB;
    tx_msg.size       = 2;
    switch( lora_err_var )  
        {
        case RX_CRC_ERROR:
            tx_msg.message[1] = 0x01;
            break;
        case RX_INVALID_HEADER:
            tx_msg.message[1] = 0x02;
            break;
        case RX_SIZING:
            tx_msg.message[1] = 0x03;
            break;
        case RX_KEY_ERR:
            tx_msg.message[1] = 0x04;
            break;
        default:
            tx_msg.message[1] = 0xFF;
            break;
        }

    send_message(tx_msg); //need to add error handling here
    }

    /*----------------------------------------------------------
    Clear errors and rx message for next round
    ----------------------------------------------------------*/
    memset( &rx_msg, 0, sizeof( rx_msg ) );
    lora_err_var = RX_NO_ERROR;
    msgRxed = false;

} /* test_mode_runtime() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_powerdown
*
*   DESCRIPTION:
*       handle disablement of test mode
*
*********************************************************************/
static void test_mode_powerdown
    (
    core::console& c            /* reference to global console      */
    )
{
    
} /*test_mode_powerdown() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_runtime
*
*   DESCRIPTION:
*       handle all test mode changes
*
*********************************************************************/
void test_mode
    (
    bool test_mode,             /* flag for if test mode is enabled */
    core::console& c            /* reference to global console      */
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
static bool test_mode_prev = false;

/*----------------------------------------------------------
Exit if test mode is not currently enabled and was not enabled
on last run of this function. This is done to allow the power
down function to be run on test mode exit
----------------------------------------------------------*/
if( test_mode == false && test_mode_prev == false )
    return;

/*----------------------------------------------------------
If this is the initial entry into test mode, assert
----------------------------------------------------------*/
if( test_mode == true && test_mode_prev == false )
    c.add_assert( "Entering test mode");

/*----------------------------------------------------------
if test mode is currently enabled, run runtime changes
----------------------------------------------------------*/
if( test_mode )
    test_mode_runtime( c );

/*----------------------------------------------------------
If test mode is disabled but was prevously enabled on the
last run of this function, run powerdown function. 
----------------------------------------------------------*/
if( test_mode == false && test_mode_prev == true )
    {
    test_mode_powerdown( c );
    c.add_assert( "Exiting test mode");
    }

/*----------------------------------------------------------
update local copy of test_mode
----------------------------------------------------------*/
test_mode_prev = test_mode;

} /* test_mode() */