/*********************************************************************
*
*   MODULE:
*       init.c
*
*   DESCRIPTION:
*       initilization process for TM4C123GH6PM
*
*   Copyright Nate Lenze 2020 
*********************************************************************/

/*--------------------------------------------------------------------
                           GENERAL INCLUDES
--------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#include "tivaware/inc/tm4c123gh6pm.h"
#include "tivaware/inc/hw_memmap.h"
#include "tivaware/inc/hw_types.h"
#include "tivaware/driverlib/sysctl.h"
#include "tivaware/driverlib/interrupt.h"
#include "tivaware/driverlib/gpio.h"
#include "tivaware/inc/hw_gpio.h"
#include "tivaware/driverlib/timer.h"
#include "tivaware/driverlib/ssi.h"
#include "tivaware/driverlib/pin_map.h"
#include "tivaware/driverlib/pwm.h"
#include "tivaware/driverlib/i2c.h"

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/
#define INTERRUPT_PORT 		( GPIO_PORTE_BASE )
#define INTERRUPT_PIN  		( GPIO_PIN_1 )

#define PART_TM4C123GH6PM  	( 1 )

#define TIMER_400_KHZ 		( SysCtlClockGet() / 100 )
#define TIMER_40_KHZ 		( SysCtlClockGet() / 1000 )
#define TIMER_4_KHZ 		( SysCtlClockGet() / 10000 )

/*-------------------------------
Enablements
-------------------------------*/
#define PORT_F_ENABLE 		( true )
#define PORT_D_ENABLE 		( false )
#define SSI_ENABLE    		( true )
#define I2C_ENABLE    		( true )
#define TIMER_1A_ENABLE 	( true )
#define INTERRUPT_ENABLE 	( true )


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
                                EXTERNS
--------------------------------------------------------------------*/
extern void Peripheral_Handler        /* interrupt handlder routine */
	(
	void
	);

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*---------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   PROCEDURE NAME:
*       sub_system_init
*
*   DESCRIPTION:
*       init all sub systems on tiva launchpad
*
*********************************************************************/
void sub_system_init 					/* initalize all subsystems */
	(
	void
	)
{
/*----------------------------------------------------------
Local variables
----------------------------------------------------------*/

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
	
/*----------------------------------------------------------
Setup bus clock to 40Mhz
----------------------------------------------------------*/
SysCtlClockSet( SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN );

/*----------------------------------------------------------
Enable Port D Outputs
----------------------------------------------------------*/
if( PORT_D_ENABLE )
	{
	/*----------------------------------------------------------
	Enable Clock on Port D
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
	while( !SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOD ) )
		{
		}
	/*----------------------------------------------------------
	Set as Outputs and strength
	----------------------------------------------------------*/
	GPIOPinTypeGPIOOutput( GPIO_PORTD_BASE, GPIO_PIN_1 | GPIO_PIN_2 );
	GPIOPadConfigSet( GPIO_PORTD_BASE,GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );

	/*----------------------------------------------------------
	Unlock Port
	----------------------------------------------------------*/
	HWREG( GPIO_PORTD_BASE + GPIO_O_LOCK ) = GPIO_LOCK_KEY;
	HWREG( GPIO_PORTD_BASE + GPIO_O_CR ) |= 0x01;
	HWREG( GPIO_PORTD_BASE + GPIO_O_LOCK ) = 0;	
	}


/*----------------------------------------------------------
Enable Port F Outputs
----------------------------------------------------------*/
if( PORT_F_ENABLE )
	{
	/*----------------------------------------------------------
	Enable Clock on Port F
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
	while( !SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOF ) )
		{
		}
	/*----------------------------------------------------------
	Set LED's as Outputs and strength ( pin 1,2,3 )
	----------------------------------------------------------*/
	GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

	/*----------------------------------------------------------
	Set switches's as inputs and strength ( pin 0,4 )
	----------------------------------------------------------*/
	GPIODirModeSet( GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN );
	GPIOPadConfigSet( GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
	/*----------------------------------------------------------
	Unlock Port
	----------------------------------------------------------*/
	HWREG( GPIO_PORTF_BASE + GPIO_O_LOCK ) = GPIO_LOCK_KEY;
	HWREG( GPIO_PORTF_BASE + GPIO_O_CR ) |= 0x01;
	HWREG( GPIO_PORTF_BASE + GPIO_O_LOCK ) = 0;
	}
		
/*----------------------------------------------------------
Enable I2C system 0
----------------------------------------------------------*/	
if( I2C_ENABLE )
	{
	/*----------------------------------------------------------
	Enable Clock on Port B
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
	SysCtlPeripheralEnable( SYSCTL_PERIPH_I2C0 );
	while( !SysCtlPeripheralReady( SYSCTL_PERIPH_I2C0 ) )
		{
		}
	/*----------------------------------------------------------
	Configure pins as I2C
	PB2 -- CLK
	PB3 -- DATA
	----------------------------------------------------------*/
	GPIOPinConfigure( GPIO_PB2_I2C0SCL );
	GPIOPinConfigure( GPIO_PB3_I2C0SDA );
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	/*----------------------------------------------------------
	Set the Launcpad as the I2C master with the slow clock 100 Kbps	
	----------------------------------------------------------*/
	I2CMasterInitExpClk(I2C0_BASE, (SysCtlClockGet()/10^7), false);

	}

/*----------------------------------------------------------
Enable SSI system 0
----------------------------------------------------------*/	
if( SSI_ENABLE )
	{
	/*----------------------------------------------------------
	Enable Clock on Port A
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOA );
	while( !SysCtlPeripheralReady( SYSCTL_PERIPH_GPIOA ) )
		{
		}

	/*----------------------------------------------------------
	Configure pins as SSI
	PA2 -- CLK
	PA4 -- RX
	PA5 -- TX
	---------
	PA3 -- CS
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_SSI0 );
	GPIOPinTypeSSI( GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_2 );
	GPIOPinConfigure( GPIO_PA2_SSI0CLK );
	GPIOPinConfigure( GPIO_PA4_SSI0RX );
	GPIOPinConfigure( GPIO_PA5_SSI0TX );

	/*----------------------------------------------------------
	Delay to allow time for setup
	----------------------------------------------------------*/
	SysCtlDelay( 20000000 );

	/*----------------------------------------------------------
	Configure A3 as chip select
	----------------------------------------------------------*/
	GPIOPinTypeGPIOOutput( GPIO_PORTA_BASE, GPIO_PIN_3 );
	GPIODirModeSet( GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT );	
	GPIOPinTypeGPIOOutput( GPIO_PORTA_BASE, GPIO_PIN_3 );
	GPIOPadConfigSet( GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
	SysCtlDelay( 20000000 );

	/*----------------------------------------------------------
	Delay to allow time for setup
	----------------------------------------------------------*/
	SysCtlDelay(20000000);

	/*----------------------------------------------------------
	Setup baudrate of 100Hz
	----------------------------------------------------------*/
	SSIConfigSetExpClk( SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 100000, 8);

	/*----------------------------------------------------------
	Toggle A3 high to start
	----------------------------------------------------------*/
	GPIO_PORTA_DATA_R |= 1<<3;

	}	
/*----------------------------------------------------------
Enable Port E1 as interrupt handler
----------------------------------------------------------*/	
if( INTERRUPT_ENABLE )
	{
	/*----------------------------------------------------------
	Enable Port E1
	----------------------------------------------------------*/
  	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE );
	GPIOPinTypeGPIOInput( GPIO_PORTE_BASE, GPIO_PIN_1 );
	GPIOPadConfigSet( GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );

	/*----------------------------------------------------------
	Configure Port E1 as interrupt
	----------------------------------------------------------*/
	GPIOIntDisable( GPIO_PORTE_BASE, GPIO_PIN_1 );
	GPIOIntClear( GPIO_PORTE_BASE, GPIO_PIN_1 );
	GPIOIntRegister( GPIO_PORTE_BASE, Peripheral_Handler );
	GPIOIntTypeSet( GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE );
	GPIOIntEnable( GPIO_PORTE_BASE, GPIO_PIN_1 );

	}

/*----------------------------------------------------------
Enable Port A1 as timer
----------------------------------------------------------*/
if( TIMER_1A_ENABLE )
	{
	/*----------------------------------------------------------
	Enable clock for Timer1 and configure for periodic interrupt
	----------------------------------------------------------*/
	SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER1 );
	TimerConfigure( TIMER1_BASE, TIMER_CFG_PERIODIC );

	/*----------------------------------------------------------
	Setup timer speed to TIMER_400_KHZ
	----------------------------------------------------------*/
	TimerLoadSet( TIMER1_BASE, TIMER_A, TIMER_400_KHZ -1 );

	/*----------------------------------------------------------
	Enable interrupts for NVIC and timer module
	----------------------------------------------------------*/
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	/*----------------------------------------------------------
	Set interrupt priority
	----------------------------------------------------------*/
	IntPrioritySet(INT_TIMER1A, 0xE0);

	/*----------------------------------------------------------
	Start timer
	----------------------------------------------------------*/
	TimerEnable(TIMER1_BASE, TIMER_A);
	}

}

