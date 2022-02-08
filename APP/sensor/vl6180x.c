/*! \file vl6180x.c
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
#include "vl6180x.h"

#if SUPPORT_VL6180X

#include "vl6180x_platform.h"
#if defined(BUILD_WITH_FULL_API_ENABLED)
   #include "vl6180x_api.h"
#else
   #include "vl6180x_api.h"
#endif

/************************************* Defines ***********************************************/
#define BIN_SENSOR_I2C_ADDRESS              ( 0x29 << 1 ) /* Sensor configuration */
#define MAX_CONVERGENCE_TIME_MSEC           20

/************************************** Types ************************************************/


/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/


/********************************** Functions Prototype **************************************/


/********************************** Functions Definition *************************************/
/**
* \name     VL6180X_pwrp
* \brief    power up sensor module
*
* \param    None
* \retval   None
*/
void VL6180X_pwrp( void )
{

}

/**
* \name     VL6180X_init
* \brief    Initialize sensor module
*
* \param    None
* \retval   None
*/
void VL6180X_init( void )
{
   VL6180x_InitData( BIN_SENSOR_I2C_ADDRESS );

   VL6180x_FilterSetState(BIN_SENSOR_I2C_ADDRESS, 0); //disable filtering as not effective in continuous mode

   VL6180x_Prepare(BIN_SENSOR_I2C_ADDRESS);

   VL6180x_UpscaleSetScaling(BIN_SENSOR_I2C_ADDRESS, 1);

   VL6180x_RangeSetInterMeasPeriod(BIN_SENSOR_I2C_ADDRESS, 0 ); // 0  will set minimal possible: 10msec
   /* Max conversion time is the sum of the Convergence Time + Readout Averaging. The default is set to 50msec
    * Readout Averaging is 4.3msec. The Convergence Time depends on the target reflectance and range.
    * According to Table 11 in data sheet ( Typical range convergence time ), for up to 100mm, the Range Conversion Time is 10.73msec.
    * We set it here to 20msec. After 20msec, measurement will be aborted.
    */
   VL6180x_RangeSetMaxConvergenceTime(BIN_SENSOR_I2C_ADDRESS, MAX_CONVERGENCE_TIME_MSEC);

   // set vl6180x gpio1 pin to range interrupt output with high polarity (rising edge)
   VL6180x_SetupGPIO1(BIN_SENSOR_I2C_ADDRESS, GPIOx_SELECT_GPIO_INTERRUPT_OUTPUT, INTR_POL_HIGH);

   VL6180x_RangeClearInterrupt( BIN_SENSOR_I2C_ADDRESS ); // make sure no interrupt is pending
}

/**
* \name     VL6180X_enableSensorInterrupt
* \brief    Enable sample ready interrupt. It initializes the GPIO interrupt on sensor and all HW related interrupts.
*
* \param    enable TRUE enables it and FALSE disables it
* \retval   None
*/
void VL6180X_enableSensorInterrupt( BOOL enable )
{
   if( enable )
   {
      VL6180x_SetGroupParamHold( BIN_SENSOR_I2C_ADDRESS, TRUE );
      VL6180x_RangeConfigInterrupt( BIN_SENSOR_I2C_ADDRESS, CONFIG_GPIO_INTERRUPT_NEW_SAMPLE_READY );
      VL6180x_SetGroupParamHold( BIN_SENSOR_I2C_ADDRESS, FALSE );
      VL6180x_ClearAllInterrupt( BIN_SENSOR_I2C_ADDRESS );
      VL6180x_RangeStartContinuousMode( BIN_SENSOR_I2C_ADDRESS );
   }
   else
   {
      VL6180x_RangeStartSingleShot( BIN_SENSOR_I2C_ADDRESS );
      VL6180x_RangeConfigInterrupt( BIN_SENSOR_I2C_ADDRESS, CONFIG_GPIO_INTERRUPT_DISABLED );
      VL6180x_ClearAllInterrupt( BIN_SENSOR_I2C_ADDRESS );
   }
}

/**
* \name     VL6180X_clearAllInterrupts
* \brief    Clear all interrupts
*
* \param    None
* \retval   None
*/
void VL6180X_clearAllInterrupts( void )
{
   VL6180x_ClearAllInterrupt( BIN_SENSOR_I2C_ADDRESS );
}

/**
* \name     VL6180X_getDistance
* \brief    Get the distance results
*
* \param    presults pointer to the results structure. It is filled by this function.
* \retval   BOOL Returns TRUE if successful
*/
BOOL VL6180X_getDistance( SENSOR_result_t *presults )
{
   VL6180x_RangeData_t rangeData;
   int8_t status;
   status = VL6180x_RangeGetMeasurement( BIN_SENSOR_I2C_ADDRESS, &rangeData );
   if (status == 0)
   {
      /*  clear range interrupt source */
      status = VL6180x_ClearAllInterrupt( BIN_SENSOR_I2C_ADDRESS );
      presults->distance = rangeData.range_mm;
      presults->rangeStatus = rangeData.errorStatus;
      presults->signalRate = rangeData.signalRate_mcps;
      return TRUE;
   }
   return FALSE;
}

/**
* \name     VL6180X_isDataReady
* \brief    checks of data is ready on sensor
*
* \param    None
* \retval   BOOL returns TRUE if data is ready
*/
BOOL VL6180X_isDataReady( void )
{
   int8_t status;
   IntrStatus_t IntStatus;

   status = VL6180x_RangeGetInterruptStatus( BIN_SENSOR_I2C_ADDRESS, &IntStatus.val );
   if( status == 0 )
   {
      if (IntStatus.status.Range == RES_INT_STAT_GPIO_NEW_SAMPLE_READY ||
      IntStatus.status.Error != 0)
      {
         return TRUE;
      }
   }
   return FALSE;
}
#endif // SUPPORT_VL6180X
