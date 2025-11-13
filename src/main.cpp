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
#include "wifi.hpp"

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

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/
core::console Console( uart0 );                     /* console API  */

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
bool             sdio_err_var; /* stdio init errors       */
pico_error_codes wifi_err_var; /* wifi init errors        */
uint8_t          i;            /* index                   */
static bool      called_once;  /* TESTING asssert flag    */

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
sdio_err_var = false;
wifi_err_var = PICO_ERROR_GENERIC;

called_once = false;

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
    Example Loop
    ------------------------------------------------------*/

	} /* while(true) */

} /* main() */