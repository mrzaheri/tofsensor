/*! \file vl53l0.c
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
#include "vl53l1.h"

#if SUPPORT_VL53L1
#include "vl53l1_platform.h"
#if defined(BUILD_WITH_FULL_API_ENABLED)
   #include "vl53l1_api.h"
#else
   #include "vl53l1X_api.h"
#endif

/************************************* Defines ***********************************************/
#define BIN_SENSOR_I2C_ADDRESS               ( 0x29 << 1 ) /* Sensor configuration */
#define BIN_SENSOR_TIMING_BUDGET_US          33000 // Note: On compact driver, limited numbers are supported. See VL53L1X_SetTimingBudgetInMs for details.
#define BIN_SENSOR_TIMING_BUDGET_MS          (BIN_SENSOR_TIMING_BUDGET_US/1000)
#define BIN_SENSOR_INTER_MEASUREMENT_MS      40 //Intermeasurement period must be minimum of TimingBudget + 4ms.
#if defined(BUILD_WITH_FULL_API_ENABLED)
   #define BIN_SENSOR_RANGE_MODE                VL53L1_DISTANCEMODE_SHORT
#else
   #define BIN_SENSOR_RANGE_MODE             1 // 1: short range. 2: long (Medium is not supported in comapct driver)
#endif

/************************************** Types ************************************************/


/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/
static VL53L1_Dev_t  vl53l1_c;

/********************************** Functions Prototype **************************************/


/********************************** Functions Definition *************************************/
void VL53L1_pwrp( void )
{

}


void VL53L1_init( void )
{

   vl53l1_c.I2cDevAddr = BIN_SENSOR_I2C_ADDRESS;
   #if defined(BUILD_WITH_FULL_API_ENABLED)
      VL53L1_software_reset(&vl53l1_c);
      VL53L1_DataInit(&vl53l1_c);
      VL53L1_StaticInit(&vl53l1_c);
      VL53L1_SetMeasurementTimingBudgetMicroSeconds(&vl53l1_c, BIN_SENSOR_TIMING_BUDGET_US);
      VL53L1_SetInterMeasurementPeriodMilliSeconds(&vl53l1_c, BIN_SENSOR_INTER_MEASUREMENT_MS);
      VL53L1_SetDistanceMode(&vl53l1_c, BIN_SENSOR_RANGE_MODE);
      VL53L1_SetPresetMode(&vl53l1_c ,VL53L1_PRESETMODE_AUTONOMOUS);
      VL53L1_StartMeasurement(&vl53l1_c);
   #else
      VL53L1X_SensorInit(vl53l1_c.I2cDevAddr);
      //VL53L1X_GetSensorId(vl53l1_c.I2cDevAddr, &sensor_id);
      VL53L1X_SetTimingBudgetInMs(vl53l1_c.I2cDevAddr, BIN_SENSOR_TIMING_BUDGET_MS);
      VL53L1X_SetInterMeasurementInMs(vl53l1_c.I2cDevAddr, BIN_SENSOR_INTER_MEASUREMENT_MS);
      VL53L1X_SetDistanceMode(vl53l1_c.I2cDevAddr, BIN_SENSOR_RANGE_MODE);
   #endif
}

uint8_t VL53L1_getDistance(SENSOR_result_t *presults)
{
   uint8_t error;
   #if defined(BUILD_WITH_FULL_API_ENABLED)
      VL53L1_RangingMeasurementData_t device_results;
      error = VL53L1_GetRangingMeasurementData(&vl53l1_c, &device_results);
      presults->distance = device_results.RangeMilliMeter;
      presults->rangeStatus = device_results.RangeStatus;
   #else
      error = VL53L1X_GetDistance(vl53l1_c.I2cDevAddr, &presults->distance);
      error |= VL53L1X_GetRangeStatus(vl53l1_c.I2cDevAddr, &presults->rangeStatus);
      error |= VL53L1X_GetSignalPerSpad(vl53l1_c.I2cDevAddr, &presults->signalRate);
      error |= VL53L1X_ClearInterrupt(vl53l1_c.I2cDevAddr);
   #endif
   return error;
}


BOOL VL53L1_isDataReady( void )
{
   BOOL ready;
   ready  = FALSE;
   #if defined(BUILD_WITH_FULL_API_ENABLED)
      VL53L1_GetMeasurementDataReady(&vl53l1_c, &ready);
   #else
      VL53L1X_CheckForDataReady(vl53l1_c.I2cDevAddr, &ready);
   #endif
   return ready;
}

void VL53L1_clearAllInterrupts( void )
{
   #if defined(BUILD_WITH_FULL_API_ENABLED)
      #error "Not Supported"
   #else
   VL53L1X_ClearInterrupt(vl53l1_c.I2cDevAddr);
   #endif
}

void VL53L1_enableSensorInterrupt( BOOL enable )
{
   if( enable )
   {
      #if defined(BUILD_WITH_FULL_API_ENABLED)
         #error "Not Supported"
      #else
         VL53L1X_SetInterruptPolarity(vl53l1_c.I2cDevAddr, 1); //  1=active high (default), 0=active low

         VL53L1X_StartRanging(vl53l1_c.I2cDevAddr); // this should be called once setup is complete.
      #endif
   }
}

#endif // SUPPORT_VL53L1
