/*! @file i2c.h
 *
 *  @brief I2C functions and variables declaration
 *
 *
 *  @author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

/********************************** Includes *******************************************/
#include "main.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void I2C_pwrp( void );

void I2C_init( void );

BOOL I2C_write( uint8_t module_address, uint8_t *data, const uint8_t data_size );

BOOL I2C_read( uint8_t module_address, uint8_t *data, const uint8_t data_size );

#endif /* I2C_I2C_H_ */
