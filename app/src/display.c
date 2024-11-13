//=====[Libraries]=============================================================

//#include "mbed.h"
//#include "arm_book_lib.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
//=====[Declaration of private defines]========================================

#define OFF    0
#define ON    1

#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 64
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 20
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 84

#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

#define DISPLAY_RW_WRITE 0
#define DISPLAY_RW_READ  1

#define DISPLAY_PIN_RS  4
#define DISPLAY_PIN_RW  5
#define DISPLAY_PIN_EN  6
#define DISPLAY_PIN_D0  7  
#define DISPLAY_PIN_D1  8  
#define DISPLAY_PIN_D2  9  
#define DISPLAY_PIN_D3 10
#define DISPLAY_PIN_D4 11
#define DISPLAY_PIN_D5 12 
#define DISPLAY_PIN_D6 13 
#define DISPLAY_PIN_D7 14 

#define DISPLAY_PIN_A_PCF8574 3

#define I2C1_SDA PB_9//PB_7
#define I2C1_SCL PB_8//PB_6

#define PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS 78

//=====[Declaration of private data types]=====================================

typedef struct{
    int address;
    char data;
    bool displayPinRs;
    bool displayPinRw; 
    bool displayPinEn;
    bool displayPinA;
    bool displayPinD4;
    bool displayPinD5;
    bool displayPinD6;
    bool displayPinD7;
} pcf8574_t;

//=====[Declaration and initialization of public global objects]===============

#define D10_Port GPIOB 
#define D10_Pin  GPIO_PIN_6

#define D11_Port GPIOA 
#define D11_Pin  GPIO_PIN_7

#define D2_Port GPIOA 
#define D2_Pin  GPIO_PIN_10

#define D12_Port GPIOA 
#define D12_Pin  GPIO_PIN_6

#define D4_Port GPIOB
#define D4_Pin  GPIO_PIN_5

#define D5_Port GPIOB
#define D5_Pin  GPIO_PIN_4

#define D6_Port GPIOB
#define D6_Pin  GPIO_PIN_10

#define D7_Port GPIOA
#define D7_Pin  GPIO_PIN_8

#define D8_Port GPIOA
#define D8_Pin  GPIO_PIN_9

#define D9_Port GPIOC
#define D9_Pin  GPIO_PIN_7
/*
DigitalOut displayD0( D10 ); //no se puede
DigitalOut displayD1( D11 ); //no se puede
DigitalOut displayD2( D2 ); //se puede
DigitalOut displayD3( D12 ); //no se puede
DigitalOut displayD4( D4 ); //se puede
DigitalOut displayD5( D5 ); //se puede
DigitalOut displayD6( D6 ); //se puede
DigitalOut displayD7( D7 ); //se puede
DigitalOut displayRs( D8 ); //se puede
DigitalOut displayEn( D9 ); //se puede

I2C i2cPcf8574( I2C1_SDA, I2C1_SCL ); 
*/
//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static display_t display;
static pcf8574_t pcf8574;
static bool initial8BitCommunicationIsCompleted;

//=====[Declarations (prototypes) of private functions]========================

static void displayPinWrite( uint8_t pinName, int value );
static void displayDataBusWrite( uint8_t dataByte );
static void displayCodeWrite( bool type, uint8_t dataBus );

//=====[Implementations of public functions]===================================

void displayInit( displayConnection_t connection )
{
    display.connection = connection;
    /*
    if( display.connection == DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER) {
        pcf8574.address = PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS;
        pcf8574.data = 0b00000000;
        //i2cPcf8574.frequency(100000);
        //displayPinWrite( DISPLAY_PIN_A_PCF8574,  ON );
    } 
    */
    initial8BitCommunicationIsCompleted = false;    
    
    HAL_Delay(50);
    //HAL_Delay( 50 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    //HAL_Delay( 5 );
    HAL_Delay(5);

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    //HAL_Delay( 1 );
    HAL_Delay(1);

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    //HAL_Delay( 1 );
    HAL_Delay(1);

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_8BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            //HAL_Delay( 1 );
            HAL_Delay(1);
        break;
        
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS );
            //HAL_Delay( 1 );
            HAL_Delay(1);


            initial8BitCommunicationIsCompleted = true;  

            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            //HAL_Delay( 1 );
            HAL_Delay(1);

        break;
    }

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF |      
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |       
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );       
    //HAL_Delay( 1 );
    HAL_Delay(1);

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_CLEAR_DISPLAY );       
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_ENTRY_MODE_SET |
                      DISPLAY_IR_ENTRY_MODE_SET_INCREMENT |       
                      DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT );                  
    HAL_Delay( 1 );

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON |      
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |    
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );    
    HAL_Delay( 1 );
}

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY )
{    
    switch( charPositionY ) {
        case 0:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            HAL_Delay( 1 );
        break;
       
        case 1:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            HAL_Delay( 1 );
        break;
       
        case 2:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            HAL_Delay( 1 );
        break;

        case 3:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            HAL_Delay( 1 );
        break;
    }
}

void displayStringWrite( const char * str )
{
    while (*str) {
        displayCodeWrite(DISPLAY_RS_DATA, *str++);
    }
}

//=====[Implementations of private functions]==================================

static void displayCodeWrite( bool type, uint8_t dataBus )
{
    if ( type == DISPLAY_RS_INSTRUCTION )
        displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION);
    else
        displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_DATA);
    displayPinWrite( DISPLAY_PIN_RW, DISPLAY_RW_WRITE );
    displayDataBusWrite( dataBus );
}

static void displayPinWrite( uint8_t pinName, int value )
{
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D0: 
                    //displayD0 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D10_Port, (uint16_t)D10_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D1: 
                    //displayD1 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D11_Port,  (uint16_t)D11_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D2: 
                    //displayD2 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D2_Port,  (uint16_t)D2_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D3: //displayD3 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D12_Port,  (uint16_t)D12_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_D4: //displayD4 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D4_Port,  (uint16_t)D4_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_D5: //displayD5 = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D5_Port,  (uint16_t)D5_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_D6: //displayD6 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D6_Port,  (uint16_t)D6_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_D7: //displayD7 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D7_Port,  (uint16_t)D7_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_RS: //displayRs = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D8_Port,  (uint16_t)D8_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_EN: //displayEn = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D9_Port,  (uint16_t)D9_Pin, (GPIO_PinState)value);
                    break;
                
                case DISPLAY_PIN_RW:
                    break; 
                
                default: break;
            }
            break;
        case DISPLAY_CONNECTION_GPIO_4BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D4: //displayD4 = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D4_Port,  (uint16_t)D4_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D5: //displayD5 = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D5_Port,  (uint16_t)D5_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D6: //displayD6 = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D6_Port,  (uint16_t)D6_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_D7: //displayD7 = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D7_Port,  (uint16_t)D7_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_RS: //displayRs = value;   
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D8_Port,  (uint16_t)D8_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_EN: //displayEn = value;  
                    HAL_GPIO_WritePin((GPIO_TypeDef*) D9_Port,  (uint16_t)D9_Pin, (GPIO_PinState)value);
                    break;
                case DISPLAY_PIN_RW: break; 
                default: break;
            }
            break;
        /*
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
           if ( value ) {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPinD4 = ON; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPinD5 = ON; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPinD6 = ON; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPinD7 = ON; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPinRs = ON; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPinEn = ON; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPinRw = ON; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPinA = ON; break;
                    default: break;
                }
            }
           else {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPinD4 = OFF; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPinD5 = OFF; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPinD6 = OFF; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPinD7 = OFF; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPinRs = OFF; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPinEn = OFF; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPinRw = OFF; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPinA = OFF; break;
                    default: break;
                }
            }     
            pcf8574.data = 0b00000000;
            if ( pcf8574.displayPinRs ) pcf8574.data |= 0b00000001; 
            if ( pcf8574.displayPinRw ) pcf8574.data |= 0b00000010; 
            if ( pcf8574.displayPinEn ) pcf8574.data |= 0b00000100; 
            if ( pcf8574.displayPinA  ) pcf8574.data |= 0b00001000; 
            if ( pcf8574.displayPinD4 ) pcf8574.data |= 0b00010000; 
            if ( pcf8574.displayPinD5 ) pcf8574.data |= 0b00100000; 
            if ( pcf8574.displayPinD6 ) pcf8574.data |= 0b01000000; 
            if ( pcf8574.displayPinD7 ) pcf8574.data |= 0b10000000; 
            i2cPcf8574.write( pcf8574.address, &pcf8574.data, 1);
            break;    
    */
    }
}

static void displayDataBusWrite( uint8_t dataBus )
{
    displayPinWrite( DISPLAY_PIN_EN, OFF );
    displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b10000000 );
    displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b01000000 );
    displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00100000 );
    displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00010000 );

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPinWrite( DISPLAY_PIN_D3, dataBus & 0b00001000 );
            displayPinWrite( DISPLAY_PIN_D2, dataBus & 0b00000100 );  
            displayPinWrite( DISPLAY_PIN_D1, dataBus & 0b00000010 );      
            displayPinWrite( DISPLAY_PIN_D0, dataBus & 0b00000001 );
        break; 
              
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            if ( initial8BitCommunicationIsCompleted == true) {
                displayPinWrite( DISPLAY_PIN_EN, ON );         
                HAL_Delay( 1 );
                displayPinWrite( DISPLAY_PIN_EN, OFF );              
                HAL_Delay( 1 );
                displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b00001000 );
                displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b00000100 );  
                displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00000010 );      
                displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00000001 );                
            }
        break;
    
    }
    displayPinWrite( DISPLAY_PIN_EN, ON );              
    HAL_Delay( 1 );
    displayPinWrite( DISPLAY_PIN_EN, OFF );  
    HAL_Delay( 1 );
}