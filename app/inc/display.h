//=====[#include guards - begin]===============================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

//=====[Declaration of public defines]=========================================
#define DISPLAY_4BITS   1
#define DISPLAY_8BITS   2
#define DISPLAY_I2C     3
#define DISPLAY_MODE (DISPLAY_8BITS)

//=====[Declaration of public data types]======================================

typedef enum {
     DISPLAY_CONNECTION_GPIO_4BITS,
     DISPLAY_CONNECTION_GPIO_8BITS,
     DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER,
} displayConnection_t;

typedef struct {
   displayConnection_t connection;                                                 
} display_t;

//=====[Declarations (prototypes) of public functions]=========================

void displayInit( displayConnection_t connection );
 
void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY );

void displayStringWrite( const char * str );

//=====[#include guards - end]=================================================

#endif // _DISPLAY_H_
