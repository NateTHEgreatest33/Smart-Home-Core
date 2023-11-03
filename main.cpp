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
#include "hardware/spi.h"
#include "pico/error.h"

extern "C" 
    {
    #include "messageAPI.h"
    }

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
#define RED_LED               ( 0x02 )      /* value for red LED    */
#define GREEN_LED             ( 0x08 )      /* value for Green LED  */
#define BLUE_LED              ( 0x04 )      /* value for Blue LED   */

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


/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/
int main                                   /* main proccessing loop */
    (
    void
    );


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
rx_message       rx_msg;
tx_message       tx_msg;
lora_errors      lora_err_var;
bool             sdio_err_var;
pico_error_codes wifi_err_var;

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

/*----------------------------------------------------------
If issue with subsystem initilization do not move forward
----------------------------------------------------------*/
if ( lora_err_var != RX_NO_ERROR || wifi_err_var != PICO_OK || !sdio_err_var )
    {

    while( true )
        {  
        }
    }

/*----------------------------------------------------------
Set LED on to signify start of main process loop
----------------------------------------------------------*/
cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, 1 );

/*----------------------------------------------------------
System Test procedure/replies
----------------------------------------------------------*/
while( true )
    {
	/*----------------------------------------------------------
    Check for new messages
    ----------------------------------------------------------*/
    // if( get_message( &rx_msg, &lora_err_var ) )
    //     {
    //     /*----------------------------------------------------------
    //     Distructive testing replies
    //     ----------------------------------------------------------*/ 
    //     if( lora_err_var != RX_NO_ERROR )
    //         {
    //         memset( &tx_msg, 0, sizeof( tx_message ) );
    //         tx_msg.destination = RPI_MODULE;

    //         tx_msg.message[0] = 0xBB;
    //         tx_msg.size       = 2;
    //         switch( lora_err_var )
    //             {
    //             case RX_CRC_ERROR:
    //                 tx_msg.message[1] = 0x01;
    //                 break;
    //             case RX_INVALID_HEADER:
    //                 tx_msg.message[1] = 0x02;
    //                 break;
    //             case RX_SIZING:
    //                 tx_msg.message[1] = 0x03;
    //                 break;
    //             case RX_KEY_ERR:
    //                 tx_msg.message[1] = 0x04;
    //                 break;
    //             default:
    //                 tx_msg.message[1] = 0xFF;
    //                 break;
    //             }
    //         send_message(tx_msg);

    //         }
    //     /*----------------------------------------------------------
    //     Non distructive testing replies
    //     ----------------------------------------------------------*/
    //     else
    //         {
    //         memset( &tx_msg, 0, sizeof( tx_message ) );
    //         tx_msg.destination = RPI_MODULE;

    //         tx_msg.message[0] = 0xAA;
    //         tx_msg.size       = 2;
    //         switch( rx_msg.size )
    //             {
    //             case 0x01:
    //                 tx_msg.message[1] = 0x11;
    //                 break;
    //             case 0x0A:
    //                 tx_msg.message[1] = 0x22;
    //                 break;
    //             default:
    //                 tx_msg.message[1] = 0xFF;
    //                 break;
    //             }
    //         send_message(tx_msg);
    //         }
    //     }
	}

} /* main() */