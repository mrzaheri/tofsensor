/*! \file hwm.h
 *
 *  \brief Declares the hwm related functions and variables
 *
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */
#ifndef __HWM_H__
#define __HWM_H__

#ifdef __cplusplus
extern "C" {
#endif
/********************************** Includes *******************************************/
#include "common.h"
#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "can.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void  HWM_pwrp( void );

void  HWM_init( void );

uint16_t HWM_getCanId( void );

#ifdef __cplusplus
}
#endif
#endif //__HWM_H__
