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

#include "messageAPI/messageAPI.h"
#include "init.c"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/
static const location current_module = MODULE_NONE;

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/
static global_error;


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
rx_message rx_msg;
tx_message tx_msg;
uint8_t lora_msg_size;

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
lora_err_var = RX_NO_ERROR;
lora_msg_size = 0;
memset( rx_msg, 0, sizeof( rx_message ) );
memset( tx_msg, 0, sizeof( tx_message ) );

/*----------------------------------------------------------
Initialize all subsystems while interrupts are disabled
----------------------------------------------------------*/
IntMasterDisable();

sub_system_init();
lora_err_var = init_message();

IntMasterEnable();

/*----------------------------------------------------------
If issue with subsystem initilization do not move forward
----------------------------------------------------------*/
if ( lora_err_var != RX_NO_ERROR )
    {
    GPIO_PORTF_DATA_R = 0x2;                                 //red
    while( true )
        {  
        }
    }

/*----------------------------------------------------------
Set LED to signify start of main process loop
----------------------------------------------------------*/
GPIO_PORTF_DATA_R = 0x08;                                   //green

/*----------------------------------------------------------
Main processing loop
----------------------------------------------------------*/
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





