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
#include "messageAPI/LoRa/LoraAPI.h"
//#include "messageAPI/messageAPI.h"
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
lora_errors lora_err_var;
lora_config lora_config_port;
uint8_t msgArr[4];
uint8_t returnMsgArr[10];
uint8_t size;
uint8_t returnSize;

msgArr[0] = 0x00;
msgArr[1] = 0x11;
msgArr[2] = 0x22;
msgArr[3] = 0x33;
size = 4;
/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
lora_err_var = RX_NO_ERROR;


lora_config_port.SSI_BASE = 0x40008000;
lora_config_port.SSI_PORT = PORT_A;
lora_config_port.SSI_PIN = 0x08;

/*----------------------------------------------------------
Initialize all subsystems while interrupts are disabled
----------------------------------------------------------*/
IntMasterDisable();

sub_system_init();
lora_port_init(lora_config_port);
lora_init_tx();



IntMasterEnable();


/*----------------------------------------------------------
Set LED to signify start of main process loop
----------------------------------------------------------*/
GPIO_PORTF_DATA_R = GREEN_LED; 

/*----------------------------------------------------------
TEST procedure
----------------------------------------------------------*/

lora_send_message( msgArr, size );

if( lora_init_continious_rx() != true)
{
	while(true)
		{
		}
}
size = 10;
while( true )
    {
    if( lora_get_message( returnMsgArr, size, &returnSize, &lora_err_var) )
			{
				while( true)
				{}
			}
	}

} /* main() */





