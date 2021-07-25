/*********************************************************************
*
*   MODULE:
*       main.c
*
*   DESCRIPTION:
*       main proccessing loops for module software
*
*   Copyright Nate Lenze 2020 
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "messageAPI/messageAPI.h"
#include "init.h"


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
static const location current_module = TIVA_MODULE;

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

void Peripheral_Handler               /* interrupt handlder routine */
    (
    void
    );


/*********************************************************************
*
*   PROCEDURE NAME:
*       Peripheral_Handler
*
*   DESCRIPTION:
*       interrupt handler for port E1
*
*********************************************************************/
void Peripheral_Handler               /* interrupt handlder routine */
    (
    void
    )
{
IntMasterDisable();

/* interrupt handler code here */

//GPIOIntClear( INTERRUPT_PORT, INTERRUPT_PIN );
IntMasterEnable();

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
rx_message rx_msg;
tx_message tx_msg;
lora_errors lora_err_var;
lora_config lora_config_port;

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
memset( &rx_msg, 0, sizeof( rx_message ) );
memset( &tx_msg, 0, sizeof( tx_message ) );

lora_err_var = RX_NO_ERROR;
lora_config_port.SSI_BASE = 0x40008000;
lora_config_port.SSI_PORT = PORT_A;
lora_config_port.SSI_PIN = 0x08;

/*----------------------------------------------------------
Initialize all subsystems while interrupts are disabled
----------------------------------------------------------*/
IntMasterDisable();

sub_system_init();
lora_err_var = init_message( lora_config_port );

IntMasterEnable();

/*----------------------------------------------------------
If issue with subsystem initilization do not move forward
----------------------------------------------------------*/
if ( lora_err_var != RX_NO_ERROR )
    {
    GPIO_PORTF_DATA_R = RED_LED;
    while( true )
        {  
        }
    }

/*----------------------------------------------------------
Set LED to signify start of main process loop
----------------------------------------------------------*/
GPIO_PORTF_DATA_R = GREEN_LED; 

/*----------------------------------------------------------
TEST procedure
----------------------------------------------------------*/
tx_msg.destination = RPI_MODULE;
tx_msg.source = current_module;
tx_msg.message[0] = 0x00;
tx_msg.message[1] = 0x11;
tx_msg.message[2] = 0x22;
tx_msg.message[3] = 0x33;
tx_msg.size = 4;

		
while( true )
    {
	/*----------------------------------------------------------
    Check for new messages
    ----------------------------------------------------------*/
    if( get_message( &rx_msg, &lora_err_var ) )
        {
        /*----------------------------------------------------------
        Proccess new messages if sent to us
        ----------------------------------------------------------*/
        if( rx_msg.destination == current_module )
            {
            /*----------------------------------------------------------
            Send Reply
            ----------------------------------------------------------*/
            send_message(tx_msg);

            }

        }
	}

} /* main() */





