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

// #include "messageAPI/messageAPI.h"

// /*--------------------------------------------------------------------
//                           LITERAL CONSTANTS
// --------------------------------------------------------------------*/
// #define RED_LED               ( 0x02 )      /* value for red LED    */
// #define GREEN_LED             ( 0x08 )      /* value for Green LED  */
// #define BLUE_LED              ( 0x04 )      /* value for Blue LED   */

// /*--------------------------------------------------------------------
//                                 TYPES
// --------------------------------------------------------------------*/

// /*--------------------------------------------------------------------
//                            MEMORY CONSTANTS
// --------------------------------------------------------------------*/
// const location current_location = PICO_MODULE;

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
// rx_message rx_msg;
// tx_message tx_msg;
// lora_errors lora_err_var;
// lora_config lora_config_port;

/*----------------------------------------------------------
Initialize local variables
----------------------------------------------------------*/
// memset( &rx_msg, 0, sizeof( rx_message ) );
// memset( &tx_msg, 0, sizeof( tx_message ) );

// lora_err_var = RX_NO_ERROR;
// lora_config_port.SSI_BASE = 0x40008000;
// lora_config_port.SSI_PORT = PORT_A;
// lora_config_port.SSI_PIN = 0x08;

/*----------------------------------------------------------
Initialize all subsystems while interrupts are disabled
----------------------------------------------------------*/
stdio_init_all();

if( cyw43_arch_init() )
    {
    printf("Wi-Fi init failed");
    return -1;
    }


// init_message( lora_config_port );

// /*----------------------------------------------------------
// If issue with subsystem initilization do not move forward
// ----------------------------------------------------------*/
// if ( lora_err_var != RX_NO_ERROR )
//     {

//     while( true )
//         {  
//         }
//     }

// /*----------------------------------------------------------
// Set LED on to signify start of main process loop
// ----------------------------------------------------------*/
cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, 1 );





#define PIN_CS   17
#define ACTIVE 0
#define INACTIVE 1

spi_init(spi_default, 100000 );
spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
// gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI );
gpio_init(PIN_CS);
gpio_set_dir(PIN_CS, GPIO_OUT);
gpio_put(PIN_CS, INACTIVE );


while(1)
    {

    sleep_ms( 1000 );

    uint8_t buffer[4]      = { 0x81, 0x80, 0x00, 0x00 }; //configure into sleep mode (0x81 -write reg 1 0x80, sleep mode)
    uint8_t rtn_buffer[4]  = { 0xFF, 0xFF, 0xFF, 0xFF }; //giberish?
    uint8_t buffer2[4]     = { 0x01, 0x00, 0x00, 0x00 }; // ( 0x01 - read from register 1)
    uint8_t rtn_buffer2[4] = { 0xFF, 0xFF, 0xFF, 0xFF }; // this should be the value 0x80



    gpio_put(PIN_CS, ACTIVE);  // Active low
    spi_write_blocking(spi_default, buffer, 4 );
    spi_read_blocking(spi_default, 0, rtn_buffer, 4);
    gpio_put(PIN_CS, INACTIVE);  // Active high




    // spi_write_blocking(spi_default, buffer2, 2 );
    gpio_put(PIN_CS, ACTIVE);  // Active high
    spi_write_read_blocking(spi_default, buffer2, rtn_buffer2, 4 );

    gpio_put(PIN_CS, INACTIVE);  // Active high


    printf(" result here ");
    }



//     msg = [0x80 | 0x01, 0x80]
//     result = spi.xfer2(msg)

//     #high power tx mode
//    # print("writing 0xFF to reg high power TX (0x09)")
//     msg = [0x80 | 0x09,0xFF]
//     result = spi.xfer2(msg)
// /*----------------------------------------------------------
// System Test procedure/replies
// ----------------------------------------------------------*/
// while( true )
//     {
// 	/*----------------------------------------------------------
//     Check for new messages
//     ----------------------------------------------------------*/
//     if( get_message( &rx_msg, &lora_err_var ) )
//         {
//         /*----------------------------------------------------------
//         Distructive testing replies
//         ----------------------------------------------------------*/ 
//         if( lora_err_var != RX_NO_ERROR )
//             {
//             memset( &tx_msg, 0, sizeof( tx_message ) );
//             tx_msg.destination = RPI_MODULE;

//             tx_msg.message[0] = 0xBB;
//             tx_msg.size       = 2;
//             switch( lora_err_var )
//                 {
//                 case RX_CRC_ERROR:
//                     tx_msg.message[1] = 0x01;
//                     break;
//                 case RX_INVALID_HEADER:
//                     tx_msg.message[1] = 0x02;
//                     break;
//                 case RX_SIZING:
//                     tx_msg.message[1] = 0x03;
//                     break;
//                 case RX_KEY_ERR:
//                     tx_msg.message[1] = 0x04;
//                     break;
//                 default:
//                     tx_msg.message[1] = 0xFF;
//                     break;
//                 }
//             send_message(tx_msg);

//             }
//         /*----------------------------------------------------------
//         Non distructive testing replies
//         ----------------------------------------------------------*/
//         else
//             {
//             memset( &tx_msg, 0, sizeof( tx_message ) );
//             tx_msg.destination = RPI_MODULE;

//             tx_msg.message[0] = 0xAA;
//             tx_msg.size       = 2;
//             switch( rx_msg.size )
//                 {
//                 case 0x01:
//                     tx_msg.message[1] = 0x11;
//                     break;
//                 case 0x0A:
//                     tx_msg.message[1] = 0x22;
//                     break;
//                 default:
//                     tx_msg.message[1] = 0xFF;
//                     break;
//                 }
//             send_message(tx_msg);
//             }
//         }
// 	}

} /* main() */