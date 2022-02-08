/*! \file main.c
 *
 *  \brief run the app
 *
 *  Initialize the system and run the app.
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "main.h"
#include "system.h"
#include "sensor.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/
typedef void (*main_events_func_callback_type)(MAIN_events_type event);

/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/
static volatile MAIN_events_type main_events;
static main_events_func_callback_type events_callback_list[MAIN_EVENTS_TOTAL] =
   {
      SENSOR_dataReadyCallback,
   };

/****************************** Functions Prototype ************************************/

/****************************** Functions Definition ***********************************/
/**
* \name     main
* \brief    Running main application
*
* \param    None
* \retval   None
*/
int main(void)
{
   uint8_t event_index;

   SYSTEM_pwrp();

   SYSTEM_init();

   while( TRUE )
   {
      while( main_events )
      {
         for( event_index = 0; event_index < MAIN_EVENTS_TOTAL; event_index++ )
         {
            MAIN_events_type event = (1 << event_index);
            if( main_events & event )
            {
               main_events &= ~event; // clear the event flag before calling the handler as the handler can set the event again if needed.
               if( events_callback_list[event_index] != NULL )
               {
                  events_callback_list[event_index](event);
               }
               break;
            }
         }
         SYSTEM_kickDog();
      }
      SYSTEM_WFI();
   }
}

/**
* \name     MAIN_signalEvent
* \brief    Signaling a main event
*
* \param    event The event that needs to be processed.
* \retval   None
*/

void MAIN_signalEvent( MAIN_events_type event )
{
   main_events |= event;
}

