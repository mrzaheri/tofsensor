/*! @file i2c_msp.c
 *
 *  @brief Defines the project specific functions for HAL I2C driver
 *
 *
 *  @author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/*-------------------------------- Includes -----------------------------------*/
#include <driver_msp_prj.h>

/*-------------------------------- Types --------------------------------------*/

/*-------------------------------- Macros -------------------------------------*/

/*-------------------------------- Variables ----------------------------------*/


/*---------------------------- Function Prototypes ----------------------------*/


/*---------------------------- Function Definitions ---------------------------*/

/**
* @name     HAL_I2C_MspInit
* @brief    Initialize the I2C MSP
*
* @param    hi2c: I2C handle
* @retval   None
*/
void HAL_I2C_MspInit( I2C_HandleTypeDef* hi2c )
{
   GPIO_InitTypeDef GPIO_init_struct;
   if( hi2c->Instance == SENSOR_I2C )
   {
      SENSOR_I2C_GPIO_CLK_ENABLE();
      GPIO_init_struct.Mode = GPIO_MODE_AF_OD;
      GPIO_init_struct.Pull = GPIO_NOPULL;
      GPIO_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_init_struct.Alternate = SENSOR_I2C_GPIO_AF;

      GPIO_init_struct.Pin = SENSOR_SCL_PIN;
      HAL_GPIO_Init( SENSOR_SCL_PORT, &GPIO_init_struct );
      GPIO_init_struct.Pin = SENSOR_SDA_PIN;
      HAL_GPIO_Init( SENSOR_SDA_PORT, &GPIO_init_struct );


      /* Peripheral clock enable */
      SENSOR_I2C_CLK_ENABLE();
      /* enable interrupts */
      //HAL_NVIC_SetPriority( SENSOR_I2C_EV_IRQn, INTERRUPT_PRIORITY_HIGH, 0 );
      //HAL_NVIC_EnableIRQ( SENSOR_I2C_EV_IRQn );
   }
}

/**
* @name     HAL_I2C_MspDeInit
* @brief    DeInitialize the UART MSP
*
* @param    hi2c: I2C handle
* @retval   None
*/
void HAL_I2C_MspDeInit( I2C_HandleTypeDef* hi2c )
{
   if( hi2c->Instance == SENSOR_I2C )
   {
      /* Peripheral clock disable */
      SENSOR_I2C_CLK_DISABLE();

      HAL_GPIO_DeInit( SENSOR_SCL_PORT, SENSOR_SCL_PIN );
      HAL_GPIO_DeInit( SENSOR_SDA_PORT, SENSOR_SDA_PIN );
      /* I2C interrupt DeInit */
      HAL_NVIC_DisableIRQ( SENSOR_I2C_EV_IRQn );
   }
}
