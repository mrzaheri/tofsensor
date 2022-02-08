/*! \file system.c
 *
 *  \brief system initialization and power up
 *
 *  Initialize the system and hardware.
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "main.h"
#include "system.h"
#include "hwm.h"
#include "sensor.h"
#include "comm.h"
#include "git_describe.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/


/****************************** Functions Prototype ************************************/


/****************************** Functions Definition ***********************************/
/**
* \name     SYSTEM_kickDog
* \brief    Power up all hardware and peripherals
*
* \param    None
* \retval   None
*/
void SYSTEM_pwrp( void )
{
   HWM_pwrp();

   DEBUG_pwrp();
   COMM_pwrp();
}

/**
* \name     SYSTEM_kickDog
* \brief    Initialize all hardware and peripherals
*
* \param    None
* \retval   None
*/
void SYSTEM_init( void )
{
    HWM_init();
    DEBUG_init();
    COMM_init();

    SENSOR_init();

    USER_LED_TOGGLE( TOTAL_STARTUP_BLINKS );

    DEBUG_LOG("%s, %s", GIT_FULL_DESCRIPTION, BUILD_CONFIG_NAME );
}

/**
* \name     SYSTEM_kickDog
* \brief    Kick the watchdog if enabled
*
* \param    None
* \retval   None
*/
void SYSTEM_kickDog( void )
{

}

/**
* \name     SYSTEM_WFI
* \brief    Wait for interrupt. System goes to sleep and waits for interrupts
*
* \param    None
* \retval   None
*/
void SYSTEM_WFI( void )
{
   __WFI();
}

/**
* \name     SYSTEM_delayMS
* \brief    Implements a delay in msec
*
* \param    delay delay in milliseconds
* \retval   None
*/
void SYSTEM_delayMS( uint32_t delay )
{
   HAL_Delay(delay);
}
