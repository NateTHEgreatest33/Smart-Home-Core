
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
#include "messageAPI.hpp"

extern "C" { 
    // #include "messageAPI.h" 
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
typedef enum
    {
    TEST_MODE_DISABLED,                   /* test mode disabled     */
    TEST_MODE_START,                      /* test mode starting up  */
    TEST_MODE_ENABLED,                    /* test mode enabled      */
    TEST_MODE_EXIT,                       /* test mode exiting      */
    NUM_TEST_MODE                         /* number of test modes   */
    }test_mode_modes;
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
extern core::messageInterface messageAPI;
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
message_errors   msg_err_var; /* MessageAPI errors        */
bool             msgRxed;  /* message rx'ed               */

/*------------------------------------------------------
Check for new messages
------------------------------------------------------*/
msgRxed = messageAPI.get_message( &rx_msg, msg_err_var );

/*------------------------------------------------------
Message API Testing:
Non distructive testing replies
------------------------------------------------------*/
if(  msgRxed == true && msg_err_var == MSG_NO_ERROR )
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
    messageAPI.send_message(tx_msg);
    }

/*----------------------------------------------------------
Distructive testing
----------------------------------------------------------*/
if( msg_err_var != RX_NO_ERROR )
    {
    memset( &tx_msg, 0, sizeof( tx_message ) );
    tx_msg.destination = RPI_MODULE;

    tx_msg.message[0] = 0xBB;
    tx_msg.size       = 2;
    switch( msg_err_var )  
        {
        case MSG_CRC_ERROR:
            tx_msg.message[1] = 0x01;
            break;
        case MSG_INVALID_HEADER:
            tx_msg.message[1] = 0x02;
            break;
        case MSG_SIZING:
            tx_msg.message[1] = 0x03;
            break;
        case MSG_KEY_ERR:
            tx_msg.message[1] = 0x04;
            break;
        default:
            tx_msg.message[1] = 0xFF;
            break;
        }

    messageAPI.send_message(tx_msg); //need to add error handling here
    }

    /*----------------------------------------------------------
    Clear errors and rx message for next round
    ----------------------------------------------------------*/
    memset( &rx_msg, 0, sizeof( rx_msg ) );
    msg_err_var = MSG_NO_ERROR;
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
*       test_mode
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
static test_mode_modes s_current_mode = TEST_MODE_DISABLED;

/*----------------------------------------------------------
Requested mode state machine
----------------------------------------------------------*/
switch( s_current_mode )
    {
    /*------------------------------
    Test mode has fully toggled off
    ------------------------------*/
    case TEST_MODE_DISABLED:
        if( test_mode )
            {
            s_current_mode = TEST_MODE_START;
            }
        break;    

    /*------------------------------
    Test mode has fully toggled on:
    Run main process
    ------------------------------*/
    case TEST_MODE_ENABLED:
        test_mode_runtime( c );
        if( !test_mode )
            {
            s_current_mode = TEST_MODE_EXIT;
            }
        break;    

    /*------------------------------
    Test mode in the process of 
    turning off. Run powerdown process
    ------------------------------*/
    case TEST_MODE_EXIT:
        test_mode_powerdown( c );
        c.add_assert( "Exiting test mode");
        s_current_mode = TEST_MODE_DISABLED;
        break;   

    /*------------------------------
    Test mode in the process of 
    turning on
    ------------------------------*/
    case TEST_MODE_START:
        c.add_assert( "Entering test mode");
        s_current_mode = TEST_MODE_ENABLED;
        break;  

    /*------------------------------
    default case
    ------------------------------*/  
    default:
        c.add_assert( "Unsupported test mode, should never get here");
        break;
    }


} /* test_mode() */