/*! \file sensor.h
 *
 *  \brief Translation layer for application level to API
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef _SENSOR_H_
#define _SENSOR_H_

/************************************ Includes ***********************************************/
#include "common.h"

/************************************* Defines ***********************************************/


/************************************** Types ************************************************/
typedef struct
{
   uint16_t distance;
   uint16_t signalRate;
   uint8_t rangeStatus;
   uint8_t comError;
   uint8_t reserved[2]; // for padding
} SENSOR_result_t;

/********************************** Global Variables *****************************************/


/********************************** Local Variables ******************************************/


/********************************** Functions Prototype **************************************/
void SENSOR_init( void );

void SENSOR_enableSensorInterrupt( BOOL enable );

void SENSOR_dataReadyCallback( MAIN_events_type events );

void SENSOR_clearAllInterrupts( void );

uint8_t SENSOR_getDistance(SENSOR_result_t *results);

BOOL SENSOR_isDataReady( void );


#endif //_SENSOR_H_
