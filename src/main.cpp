/*********************************************************************
*
*   MODULE:
*       main.cpp
*
*   DESCRIPTION:
*       main proccessing loops for module software on raspberry pi
*       pico
*
*   Copyright Nate Lenze 2023
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/error.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "background_task.hpp"
#include "button.hpp"
#include "console.hpp"
#include "messageAPI.hpp"
#include "mailbox.hpp"

#include "test_mode.hpp"
#include "mailbox_map.hpp"

#include <iostream>

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/
const location current_location = PICO_MODULE;

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/
core::console Console( uart0 );                     /* console API  */
core::loraInterface loRa( spi_default, Console );   /* Lora API     */
core::messageInterface messageAPI( loRa, Console ); /* Message API  */
core::mailbox< (size_t)mbx_index::NUM_MAILBOX > mailbox( global_mailbox );

/*--------------------------------------------------------------------
                               GLOBALS
--------------------------------------------------------------------*/
bool g_test_mode_enable = false;          /* toggle for enabling test
                                             mode, this is toggled
                                             using the consoleAPI   */

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/
int main                                   /* main proccessing loop */
    (
    void
    );

void gpio_int_handler                     /* GPIO interrupt handler */
    (
    uint gpio, 
    uint32_t events
    );

/*********************************************************************
*
*   PROCEDURE NAME:
*       gpio_int_handler
*
*   DESCRIPTION:
*       GPIO Interupt Handler
*
*   NOTES:
*       Pi Pico architecture only allows for one gpio interrupt
*       handler. It is mandated to pass in the gpio port # and the
*       event which describes the manor of the interrupt (rising edge,
*       lowering edge, etc. )
*
*********************************************************************/
void gpio_int_handler
    (
    uint gpio,                        /* gpio port number           */    
    uint32_t events                   /* event for gpio interrupt   */
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/

/*----------------------------------------------------------
Handle GPIO configured
----------------------------------------------------------*/
switch( gpio )
    {
    default:
        Console.add_assert( "GPIO Handler called on unsupported pin" );
        break;
    }
}

/*********************************************************************
*
*   PROCEDURE NAME:
*       main
*
*   DESCRIPTION:
*       main proccessing loop for module
*
*********************************************************************/
int main
    (
    void
    )
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/
rx_multi         rx_msg;    /* returned message structure */
tx_message       tx_msg;    /* transmit message structure */
bool             sdio_err_var; /* stdio init errors       */
pico_error_codes wifi_err_var; /* wifi init errors        */
uint8_t          i;            /* index                   */

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
memset( &rx_msg, 0, sizeof( rx_message ) );
memset( &tx_msg, 0, sizeof( tx_message ) );

sdio_err_var = false;
wifi_err_var = PICO_ERROR_GENERIC;

/*----------------------------------------------------------
Initialize all subsystems   
----------------------------------------------------------*/
sdio_err_var = stdio_init_all();
wifi_err_var = (pico_error_codes) cyw43_arch_init();

gpio_set_irq_callback( gpio_int_handler );

/*----------------------------------------------------------
If issue with subsystem initilization do not move forward
----------------------------------------------------------*/
if ( wifi_err_var != PICO_OK || !sdio_err_var )
    {
    Console.add_assert( "System unable to initilize" );

    while( true )
        {
        /*--------------------------------------------------
        Allow user to retrive error messages if unable to
        initilize.
        --------------------------------------------------*/
        Console.console_runtime();  
        }
    }

/*----------------------------------------------------------
launch background_task on core 1
----------------------------------------------------------*/
multicore_launch_core1( background_task );

/*----------------------------------------------------------
Set LED on to signify start of main process loop
----------------------------------------------------------*/
cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, 1 );

/*----------------------------------------------------------
System Test procedure/replies
----------------------------------------------------------*/
while( true )
    {
	/*------------------------------------------------------
    Handle test mode. and do not run main processing while 
    test mode is enabled. 

    Test mode is primarly for system level functionality
    ------------------------------------------------------*/
    test_mode( g_test_mode_enable, Console ) ;

    if( g_test_mode_enable )
        continue;

	/*------------------------------------------------------
    Check for new messages
    ------------------------------------------------------*/
    rx_msg = messageAPI.get_multi_message();
    
    /*------------------------------------------------------
    Example TX message for each rx message
    ------------------------------------------------------*/
    for( i = 0; i < rx_msg.num_messages; i++ )
        {
        memset( &tx_msg, 0, sizeof( tx_message ) );
        
        tx_msg.destination = rx_msg.messages[i].source;
        tx_msg.message[0]  = 0xFF;
        tx_msg.message[1]  = 0xFF;
        tx_msg.size        = 2;

        if( !messageAPI.send_message(tx_msg) )
            {
            Console.add_assert("MessageAPI send message failed!");
            }
        }

	} /* while(true) */

} /* main() */