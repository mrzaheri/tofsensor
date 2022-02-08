/*! \file system.h
 *
 *  \brief system initialization and power up
 *
 *  Initialize the system and hardware.
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__
/********************************** Includes *******************************************/
#include "common.h"

/*********************************** Consts ********************************************/

/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void SYSTEM_pwrp( void );

void SYSTEM_init( void );

void SYSTEM_kickDog( void );

void SYSTEM_WFI( void );

void SYSTEM_delayMS( uint32_t delay );

#endif /* __SYSTEM_H__ */
