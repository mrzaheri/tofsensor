/*! \file timer.c
 *
 *  \brief functions definitions for initializing the timer peripheral
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "timer.h"

/*-------------------------------- Types --------------------------------------*/
typedef struct
{
   MAIN_events_type callbackEvent; /* Timeout callback main event */
   uint16_t timeoutMsec;         /* original time stored for reload */
   uint16_t counter;             /* time that timer will expire */
   BOOL continuous;              /* 1= never expire, 0 = one time only */
   BOOL inUse;                   /* if it is in Use */
} timeoutHandle_t;

/*-------------------------------- Macros -------------------------------------*/


/*-------------------------------- Consts -------------------------------------*/
#define DECREMENT_EVENT_COUNTER_MSEC              1
#define TIMER_TOTAL_EVENTS                        5

/*-------------------------------- Variables ----------------------------------*/
static volatile timeoutHandle_t timers[TIMER_TOTAL_EVENTS];
static volatile uint32_t timestampMsec;

/*---------------------------- Function Prototypes ----------------------------*/

/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/

/****************************** Functions Prototype ************************************/

/****************************** Functions Definition ***********************************/

/**
* \name     TIMER_pwrp
* \brief    Timer powerup
*
* \param    None
* \retval   None
*/
void TIMER_pwrp( void )
{
   memset( (void *)timers, 0, sizeof( timers ) );
   timestampMsec = 0;
}

/**
* \name     TIMER_init
* \brief    Timer initialization
*
* \param    None
* \retval   None
*/
void TIMER_init( void )
{

}

/**
* \name     TIMER_setTimeout
* \brief    set a timeout timer to for a main event
*
* \param    timeoutMsec timeout in milliseconds
* \param    continuous if TRUE, it would set the timer continuously
* \param    callbackEvent the main event signaled on timeout
* \retval   TIMER_events_index_type timer index for future reference
*/
TIMER_events_index_type TIMER_setTimeout( uint16_t timeoutMsec, BOOL continuous, MAIN_events_type callbackEvent )
{
   if( timeoutMsec == 0 )
   {
      return TIMER_INVALID_TIMEOUT_INDEX;
   }
   /* timeout always must be a multiple of DECREMENT_EVENT_COUNTER_MSEC. So it is rounded down here: */
   timeoutMsec = (uint16_t)( ( (uint32_t)timeoutMsec / DECREMENT_EVENT_COUNTER_MSEC ) * DECREMENT_EVENT_COUNTER_MSEC );
   for( uint8_t i = 0; i < TIMER_TOTAL_EVENTS; i++ )
   {
      if( timers[i].inUse == FALSE )
      {
         timers[i].timeoutMsec = timeoutMsec;
         timers[i].counter = timeoutMsec;
         timers[i].continuous = continuous;
         timers[i].callbackEvent = callbackEvent;
         timers[i].inUse = TRUE;
         return i;
      }
   }
   DEBUG_LOG("No more available timeout handles");
   return TIMER_INVALID_TIMEOUT_INDEX;
}

/**
* \name     TIMER_checkTimeoutEvents
* \brief    Called from system tick to decrement the timeout handles if enabled.
*
* \param    None
* \retval   None
*/
void TIMER_checkTimeoutEvents( void )
{
   timestampMsec++;
   for( uint8_t i = 0; i < TIMER_TOTAL_EVENTS; i++ )
   {
      if( timers[i].inUse == TRUE )
      {
         timers[i].counter -= DECREMENT_EVENT_COUNTER_MSEC;
         /*---- if timer counted down to zero, then it expired ----*/
         if( timers[i].counter == 0 )
         {
            /* set up the timer before calling into the function to prevent any race condition */
            if( timers[i].continuous )
            {
               /* reload it */
               timers[i].counter = timers[i].timeoutMsec;
            }
            else
            {
               timers[i].inUse = FALSE;
            }
            /* Note that this function is called from system tick interrupt and only must set main events to handle the task */
            MAIN_signalEvent( timers[i].callbackEvent );
         }
      }
   }
}

/**
* \name     TIMER_getTimeMsec
* \brief    Returns the current timestamp in milliseconds. It is just relative system tick time.
*
* \param    None
* \retval   uint32_t current system time in milliseconds
*/
uint32_t TIMER_getSystemTimeMsec( void )
{
   return timestampMsec;
}


