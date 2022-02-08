/*! \file main.h
 *
 *  \brief Main app functions and variables decleration
 *
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __MAIN_H__
#define __MAIN_H__
/********************************** Includes *******************************************/
#include "common.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/
typedef uint32_t MAIN_events_type;
enum
{
   MAIN_EVENT_SENSOR_DATA_READY_BIT = 0,
   MAIN_EVENTS_TOTAL,
};

#define MAIN_EVENT_SENSOR_DATA_READY ( 1u << MAIN_EVENT_SENSOR_DATA_READY_BIT )

/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void MAIN_signalEvent( MAIN_events_type event );

#endif /* __MAIN_H__ */

