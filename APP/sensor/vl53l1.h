/*! \file vl53l0.h
 *
 *  \brief Translation layer for application level to API
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef _VL53L1_H_
#define _VL53L1_H_

/************************************ Includes ***********************************************/
#include "common.h"
#include "vl53l1_types.h"
#include "sensor.h"

/************************************* Defines ***********************************************/

/************************************** Types ************************************************/


/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/


/********************************** Functions Prototype **************************************/
void VL53L1_pwrp( void );

void VL53L1_init( void );

uint8_t VL53L1_getDistance(SENSOR_result_t *results);

BOOL VL53L1_isDataReady( void );

void VL53L1_clearAllInterrupts( void );

void VL53L1_enableSensorInterrupt( BOOL enable );

#endif //_VL53L1_H_
