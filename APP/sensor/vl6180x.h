/*! \file vl6180x.h
 *
 *  \brief Translation layer for application level to API
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef _VL6180X_H_
#define _VL6180X_H_

/************************************ Includes ***********************************************/
#include "vl6180x_types.h"
#include "sensor.h"

/************************************* Defines ***********************************************/


/************************************** Types ************************************************/


/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/


/********************************** Functions Prototype **************************************/
void VL6180X_pwrp( void );

void VL6180X_init( void );

void VL6180X_enableSensorInterrupt( BOOL enable );

void VL6180X_clearAllInterrupts( void );

BOOL VL6180X_getDistance(SENSOR_result_t *results);

BOOL VL6180X_isDataReady( void );


#endif //_VL6180X_H_
