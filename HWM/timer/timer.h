/*! \file timer.h
 *
 *  \brief TIMER module functions declarations
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __TIMER_H__
#define __TIMER_H__
/********************************** Includes *******************************************/
#include "main.h"

/*********************************** Consts ********************************************/
#define TIMER_INVALID_TIMEOUT_INDEX      0xFF

/************************************ Types ********************************************/
typedef uint8_t TIMER_events_index_type;

/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void TIMER_pwrp( void );

void TIMER_init( void );

void TIMER_checkTimeoutEvents( void );

TIMER_events_index_type TIMER_setTimeout( uint16_t timeoutMsec, BOOL continuous, MAIN_events_type callbackEvent );

uint32_t TIMER_getSystemTimeMsec( void );

#endif /* __TIMER_H__ */
