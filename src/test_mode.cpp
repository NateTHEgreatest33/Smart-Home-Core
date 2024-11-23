
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
#include "queue.hpp"

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
rx_multi         rx_msg;    /* returned message structure */
tx_message       tx_msg;    /* transmit message structure */
uint8_t          i;         /* index                      */
uint16_t         sleep;     /* sleep variable             */
utl::queue<MAX_MSG_RX, tx_message> tx_queue; 
                            /* queue for transmitting msg */

/*----------------------------------------------------------
Init local variables
----------------------------------------------------------*/
sleep             = 0;

/*------------------------------------------------------
Check for new messages
------------------------------------------------------*/
rx_msg = messageAPI.get_multi_message();

/*------------------------------------------------------
Handle each message
------------------------------------------------------*/
for( i = 0; i < rx_msg.num_messages; i++ )
    {
    /*------------------------------------------------------
    Message API Testing:
    setup common data 
    ------------------------------------------------------*/
    memset( &tx_msg, 0, sizeof( tx_message ) );
    tx_msg.destination = RPI_MODULE;
    tx_msg.size       = 2 ;

    /*------------------------------------------------------
    Non distructive testing replies
    ------------------------------------------------------*/
    if( rx_msg.errors[i] == MSG_NO_ERROR && rx_msg.global_errors == MSG_NO_ERROR )
        {
        tx_msg.message[0] = 0xAA;

        switch( rx_msg.messages[i].size )
            {
            case 0x01:
                tx_msg.message[1] = 0x11;
                break;
            case 0x02:
                tx_msg.message[1] = rx_msg.messages[i].message[0];
                break;
            case 0x03:
                /*--------------------------------
                multi-message testing requires not
                reading from the lora transciver
                hence the 5 second sleep. 
                
                0x44 is confirmation, otherwise we
                echo message[1]
                ---------------------------------*/
                if( rx_msg.messages[i].message[1] == 0x00 )
                    {
                    sleep = 5000;
                    tx_msg.message[1] = 0x44;
                    }
                else
                    {
                    tx_msg.message[1] = rx_msg.messages[i].message[1];
                    }
                break;
            case 0x0A:
                tx_msg.message[1] = 0x22;
                break;
            default:
                tx_msg.message[1] = 0xFF;
                break;
            }
        }

    /*----------------------------------------------------------
    Distructive testing
    ----------------------------------------------------------*/
    if( rx_msg.errors[i] != MSG_NO_ERROR || rx_msg.global_errors != MSG_NO_ERROR )
        {
        tx_msg.message[0] = 0xBB;

        switch( rx_msg.errors[i] )  
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
        }

    tx_queue.push( tx_msg );
    
    }

/*----------------------------------------------------------
Handle global errors if they exist
----------------------------------------------------------*/
if( rx_msg.global_errors != MSG_NO_ERROR)
    {
    /*------------------------------------------------------
    Message API Testing:
    setup common data 
    ------------------------------------------------------*/
    memset( &tx_msg, 0, sizeof( tx_message ) );
    tx_msg.destination = RPI_MODULE;
    tx_msg.size       = 2 ;

    tx_msg.message[0] = 0xBB;
    tx_msg.message[1] = 0xFF;

    if( rx_msg.global_errors == MSG_SIZING )
        tx_msg.message[1] = 0x05;
        
    if( rx_msg.global_errors == MSG_INVALID_HEADER )
        tx_msg.message[1] = 0x06;

    tx_queue.push( tx_msg );
    }

/*----------------------------------------------------------
transmit tx queue until empty
----------------------------------------------------------*/
while( !tx_queue.is_empty() )
    {
    messageAPI.send_message( tx_queue.front() );
    tx_queue.pop();
    }

/*----------------------------------------------------------
sleep if requested
----------------------------------------------------------*/
if( sleep != 0 )
    sleep_ms( sleep );
    
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