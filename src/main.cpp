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

#include "button.hpp"
#include "console.hpp"
#include "background_task.hpp"
#include "test_mode.hpp"

#include <iostream>

extern "C" 
    {
    #include "messageAPI.h"
    }

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
core::console console( uart0 );            /* console API           */

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
        console.add_assert( "GPIO Handler called on unsupported pin" );
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
rx_message       rx_msg;    /* returned message structure */
tx_message       tx_msg;    /* transmit message structure */
lora_errors      lora_err_var; /* lora errors             */
bool             sdio_err_var; /* stdio init errors       */
pico_error_codes wifi_err_var; /* wifi init errors        */
bool             msgRxed;  /* message rx'ed               */

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
memset( &rx_msg, 0, sizeof( rx_message ) );
memset( &tx_msg, 0, sizeof( tx_message ) );

lora_err_var = RX_NO_ERROR;
sdio_err_var = false;
wifi_err_var = PICO_ERROR_GENERIC;

/*----------------------------------------------------------
Initialize all subsystems   
----------------------------------------------------------*/
sdio_err_var = stdio_init_all();
wifi_err_var = (pico_error_codes) cyw43_arch_init();
lora_err_var = init_message( spi_default );

gpio_set_irq_callback( gpio_int_handler );

/*----------------------------------------------------------
If issue with subsystem initilization do not move forward
----------------------------------------------------------*/
if ( lora_err_var != RX_NO_ERROR || wifi_err_var != PICO_OK || !sdio_err_var )
    {
    console.add_assert( "System unable to initilize" );

    while( true )
        {
        /*--------------------------------------------------
        Allow user to retrive error messages if unable to
        initilize.
        --------------------------------------------------*/
        console.console_runtime();  
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
    test_mode( g_test_mode_enable, console ) ;

    if( g_test_mode_enable )
        continue;
        
	/*------------------------------------------------------
    Check for new messages
    ------------------------------------------------------*/
    msgRxed = get_message( &rx_msg, &lora_err_var );
    
    /*------------------------------------------------------
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

	} /* while(true) */

} /* main() */