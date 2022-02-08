/*! \file sensor.c
 *
 *  \brief Translation layer for application level to API
 *
 *  This is to communicate with the drivers (compact or full). The full driver can be built by "THUMB Release w Full API".
 *  The compact driver is faster and needs less memory. The full API is included for calibration if needed.
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/************************************ Includes ***********************************************/
#include "sensor.h"
#include "comm.h"
#if SUPPORT_VL6180X
   #include "vl6180x.h"
#else
   #include "vl53l1.h"
#endif
#if SUPPORT_VL6180X
   #include "vl6180x_types.h"
#else
   #include "vl53l1_types.h"
#endif

/************************************* Consts ***********************************************/
#define SENSOR_ENABLE_PIN_TOGGLE_TIME_MSEC      10

#if SUPPORT_VL53L1
   #define ENABLE_WORKAROUND_FOR_VL53L1_INTERRUPT  1/* This is a temporary fix to the interrupt issue on VL53L1 sensor. Its interrupt will not be enabled */
   #define VL53L1_WORKAROUND_CALLBACK_TIMEOUT_MSEC  5
#endif

/************************************* Macros ***********************************************/
#define DISABLE_CHIP()                      HAL_GPIO_WritePin(SENSOR_CE_PORT, SENSOR_CE_PIN, GPIO_PIN_RESET);
#define ENABLE_CHIP()                       HAL_GPIO_WritePin(SENSOR_CE_PORT, SENSOR_CE_PIN, GPIO_PIN_SET);

/************************************** Types ************************************************/


/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/


/********************************** Functions Prototype **************************************/


/********************************** Functions Definition *************************************/
/**
* \name     SENSOR_pwrp
* \brief    Power up sensor module
*
* \param    None
* \retval   None
*/
void SENSOR_pwrp( void )
{
   #if SUPPORT_VL6180X
      VL6180X_pwrp();
   #else
      VL53L1_pwrp();
   #endif
}

/**
* \name     SENSOR_init
* \brief    Initialize sensor module
*
* \param    None
* \retval   None
*/
void SENSOR_init( void )
{
   GPIO_InitTypeDef  GPIO_InitStruct;
   SENSOR_GPIO_CLK_ENABLE();

   GPIO_InitStruct.Pin       = SENSOR_CE_PIN;
   GPIO_InitStruct.Pull      = GPIO_NOPULL;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
   GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
   HAL_GPIO_Init( SENSOR_CE_PORT, &GPIO_InitStruct );

   GPIO_InitStruct.Pin       = SENSOR_INT_PIN;
   GPIO_InitStruct.Pull      = GPIO_PULLUP;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
   GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
   HAL_GPIO_Init( SENSOR_INT_PORT, &GPIO_InitStruct );

   DISABLE_CHIP();
   DELAY_MSEC(SENSOR_ENABLE_PIN_TOGGLE_TIME_MSEC);
   ENABLE_CHIP();
   DELAY_MSEC(SENSOR_ENABLE_PIN_TOGGLE_TIME_MSEC);

   #if SUPPORT_VL6180X
      VL6180X_init();
      SENSOR_enableSensorInterrupt( TRUE );
   #else
      VL53L1_init();
      SENSOR_enableSensorInterrupt( TRUE );
   #endif
}

/**
* \name     SENSOR_enableSensorInterrupt
* \brief    Enable/Disable sensor sample ready interrupt. It initializes the GPIO interrupt on sensor and all HW related interrupts.
*
* \param    enable TRUE enables it and FALSE disables it
* \retval   None
*/
void SENSOR_enableSensorInterrupt( BOOL enable )
{
   GPIO_InitTypeDef gpioInit;
   #if SUPPORT_VL6180X
      VL6180X_enableSensorInterrupt( enable );
   #else
      VL53L1_enableSensorInterrupt( enable );
      #if ENABLE_WORKAROUND_FOR_VL53L1_INTERRUPT
         // Continuously set MAIN_EVENT_SENSOR_DATA_READY event every VL53L1_WORKAROUND_CALLBACK_TIMEOUT_MSEC
         TIMER_setTimeout( VL53L1_WORKAROUND_CALLBACK_TIMEOUT_MSEC, TRUE, MAIN_EVENT_SENSOR_DATA_READY );
      #endif
   #endif

    if( enable )
    {
       gpioInit.Pin = SENSOR_INT_PIN;
       gpioInit.Mode = GPIO_MODE_IT_RISING;
       gpioInit.Pull = GPIO_NOPULL;
       HAL_GPIO_Init( SENSOR_INT_PORT, &gpioInit );

       __HAL_GPIO_EXTI_CLEAR_IT( SENSOR_INT_PIN );
       HAL_NVIC_SetPriority( SENSOR_INT_IRQn, INTERRUPT_PRIORITY_HIGH, 0 );
       HAL_NVIC_EnableIRQ( SENSOR_INT_IRQn );
    }
    else
    {
       HAL_NVIC_DisableIRQ( SENSOR_INT_IRQn );
    }
}

/**
* \name     SENSOR_dataReadyCallback
* \brief    Data ready callback function from main context triggered by interrupts
*
* \param    events passed by main context
* \retval   None
*/
void SENSOR_dataReadyCallback( MAIN_events_type events )
{
   SENSOR_result_t results;
   PARAMETER_NOT_USED( events );

   #if ENABLE_WORKAROUND_FOR_VL53L1_INTERRUPT
      if( !SENSOR_isDataReady() )
      {
         return;
      }
   #endif
   results.comError = SENSOR_getDistance( &results );

   COMM_SNSR_message_t commMsg;
   /* send it out */
   commMsg.header.morePackets = 0;
   commMsg.header.msgID = COMM_SNSR_RANGE_SENSOR_DATA_ID;
   commMsg.header.msgSize = sizeof( COMM_SNSR_RANGE_data_t );
   commMsg.payload.rangeData.distance = results.distance;
   commMsg.payload.rangeData.signalRate = results.signalRate;
   commMsg.payload.rangeData.error = results.rangeStatus;
   COMM_send( &commMsg );
}

/**
* \name     SENSOR_clearAllInterrupts
* \brief    Clear all interrupts in sensor
*
* \param    None
* \retval   None
*/
void SENSOR_clearAllInterrupts( void )
{
   #if SUPPORT_VL6180X
      VL6180X_clearAllInterrupts();
   #else
      VL53L1_clearAllInterrupts();
   #endif
}

/**
* \name     SENSOR_getDistance
* \brief    Get the distance data
*
* \param    presults pointer to the results structure. It is filled by this function.
* \retval   uint8_t returns error if there is any ( 0 means success )
*/
uint8_t SENSOR_getDistance( SENSOR_result_t *presults )
{
   #if SUPPORT_VL6180X
      return VL6180X_getDistance( presults );
   #else
      return VL53L1_getDistance( presults );
   #endif
}

/**
* \name     SENSOR_isDataReady
* \brief    check if sensor data is ready
*
* \param    None
* \retval   BOOL returns TRUE if data is ready
*/
BOOL SENSOR_isDataReady( void )
{
   #if SUPPORT_VL6180X
      return VL6180X_isDataReady();
   #else
      return VL53L1_isDataReady();
   #endif
}

