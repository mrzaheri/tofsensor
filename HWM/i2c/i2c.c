/*! @file i2c.c
 *
 *  @brief Initialize and run I2C
 *
 *
 *  @author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "i2c.h"

/*********************************** Consts ********************************************/
#define IC2_TIMEOUT         100

/************************************ Types ********************************************/

/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/
static I2C_HandleTypeDef sensor_i2c_h;

/****************************** Functions Prototype ************************************/


/****************************** Functions Definition ***********************************/

/**
* @name     I2C_pwrp
* @brief    Power up I2C module
*
* @param    None
* @retval   None
*/
void I2C_pwrp( void )
{

}

/**
* @name     I2C_init
* @brief    Initialize I2C module
*
* @param    None
* @retval   None
*/
void I2C_init( void )
{
   sensor_i2c_h.Instance = SENSOR_I2C;
   sensor_i2c_h.Init.Timing = 0x00702991;
   sensor_i2c_h.Init.OwnAddress1 = 0;
   sensor_i2c_h.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
   sensor_i2c_h.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
   sensor_i2c_h.Init.OwnAddress2 = 0;
   sensor_i2c_h.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
   sensor_i2c_h.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
   sensor_i2c_h.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

   if ( HAL_I2C_Init( &sensor_i2c_h ) != HAL_OK )
   {
      Error_Handler();
   }

   //Configure Analog filter
   if ( HAL_I2CEx_ConfigAnalogFilter( &sensor_i2c_h, I2C_ANALOGFILTER_ENABLE ) != HAL_OK )
   {
      Error_Handler();
   }

   //Configure Digital filter
   if ( HAL_I2CEx_ConfigDigitalFilter( &sensor_i2c_h, 0 ) != HAL_OK )
   {
      Error_Handler();
   }
}

/**
* @name     I2C_write
* @brief    Write into I2C module
*
* @param    module_address: 8-bit address of device being written into
* @param    data: pointer to data being written
* @param    data_size: number of bytes being written
* @retval   BOOL true if it goes fine.
*/
BOOL I2C_write( uint8_t module_address, uint8_t *data, const uint8_t data_size )
{
   HAL_StatusTypeDef retVal = HAL_I2C_Master_Transmit( &sensor_i2c_h, module_address, data, data_size, IC2_TIMEOUT );
   if( HAL_OK == retVal )
   {
       return TRUE;
   }
   else
   {
       DEBUG_LOG("I2C W error (%d)", retVal );
       return FALSE;
   }
}

/**
* @name     I2C_read
* @brief    Read from I2C module
*
* @param    module_address: 8-bit address of device being read from
* @param    data: pointer to location where the data read will be stored
* @param    data_size: number of bytes being read
* @retval   BOOL true if it goes fine.
*/
BOOL I2C_read( uint8_t module_address, uint8_t *data, const uint8_t data_size )
{
   HAL_StatusTypeDef retVal = HAL_I2C_Master_Receive( &sensor_i2c_h, module_address, data, data_size, IC2_TIMEOUT );
   if( HAL_OK == retVal )
   {
      return TRUE;
   }
   else
   {
      DEBUG_LOG("I2C R error (%d)", retVal );
      return FALSE;
   }
}


/**
* @name     I2C3_EV_IRQHandler
* @brief    This function handles I2C3 event interrupt
*
* @param    None
* @retval   None
*/
void I2C3_EV_IRQHandler( void )
{
   HAL_I2C_EV_IRQHandler( &sensor_i2c_h );
}
