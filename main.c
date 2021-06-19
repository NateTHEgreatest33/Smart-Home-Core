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

GPIOIntClear( INTERRUPT_PORT, INTERRUPT_PIN );
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
lora_errors lora_err_var;
rx_message  rx_msg;
tx_message  tx_msg;
lora_config lora_config_port;
uint8_t     i;

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
lora_err_var = RX_NO_ERROR;
memset( &rx_msg, 0, sizeof( rx_message ) );
memset( &tx_msg, 0, sizeof( tx_message ) );
memset( &tx_msg, 0, sizeof( lora_config_port ) );

lora_config_port.SSI_BASE = SSI0_BASE;
lora_config_port.SSI_PIN = 0x08;
lora_config_port.SSI_PORT = GPIO_PORTA_DATA_R;

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

/*----------------------
Setup TX message
----------------------*/
tx_msg.destination = RPI_MODULE;
tx_msg.size        = 4;
for ( i = 0; i < tx_msg.size; i++ )
    {
    tx_msg.message[i] = i;
    }

/*----------------------
Send test message
----------------------*/
lora_err_var = send_message(tx_msg);

/*----------------------
error checking
----------------------*/
if ( lora_err_var != RX_NO_ERROR )
    {
    GPIO_PORTF_DATA_R = RED_LED;
    while( true )
        {  
        }
    }

/*----------------------------------------------------------
Playback Test loop
----------------------------------------------------------*/
while( true )
    {
    
    /*----------------------
    Wait for reply
    ----------------------*/
    while( get_message( &rx_msg, &lora_err_var ) == false && rx_msg.destination != current_module )
        {
        }

    /*----------------------
    Build playback message
    ----------------------*/
    memset( &tx_msg.message, 0, sizeof( tx_msg.message ) );
    for( i = 0; i < rx_msg.size; i++ )
        {
        tx_msg.message[i] = rx_msg.message[i];
        }
    tx_msg.size = rx_msg.size;

    /*----------------------
    Send reply
    ----------------------*/
    send_message(tx_msg);
    
    }

} /* main() */





