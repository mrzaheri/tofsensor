/*! \file debug.c
 *
 *  \brief FOr printf's and debug messages
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "debug.h"
#include "uart.h"
#include "comm.h"
#include "comm_snsr_defs.h"
#include <stdarg.h>

/*********************************** Consts ********************************************/
#define MAX_DEBUG_MSG_SIZE          128

/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/
static DEBUG_verboseLevel_t systemVerboseLevel;
static uint8_t debugBuff[MAX_DEBUG_MSG_SIZE];

/****************************** Functions Prototype ************************************/
static void writeOut( uint8_t *data, uint16_t size );

/****************************** Functions Definition ***********************************/
/**
* \name     DEBUG_pwrp
* \brief    power up the debug module and resources
*
* \param    None
* \retval   None
*/
void DEBUG_pwrp( void )
{
   systemVerboseLevel = DEBUG_VERBOSE_LEVEL_INFO;
}

/**
* \name     DEBUG_init
* \brief    Initialize the debug module
*
* \param    None
* \retval   None
*/
void DEBUG_init( void )
{
   UART_init( UART_DEBUG_PORT, NULL );
}

/**
* \name     DEBUG_log
* \brief    Log debug message
*
* \param    logLevel the importance level of the message passed
* \param    file the file name the message is logged from
* \param    line the line number the message is logged from
* \param    msg the message to be logged
* \retval   None
*/
void DEBUG_logMsg( DEBUG_verboseLevel_t logLevel, uint8_t* file, int32_t line, const char *msg, ... )
{
   int32_t writtenBytes = 0;
   int32_t retCode = 0;
   char *buff;
   buff = (char *)debugBuff;
   if( logLevel <= systemVerboseLevel )
   {
      if( logLevel < DEBUG_VERBOSE_LEVEL_INFO )
      {
         /* only print out the filename and line number for critical and warnings */
         retCode = snprintf(buff, MAX_DEBUG_MSG_SIZE, "%s,%d:", file, (int)line);
         if( retCode < 0 )
         {
            return; /* something is wrong. We cannot assert here as it will call this function. */
         }
         writtenBytes += retCode;
         buff += retCode;
         if( writtenBytes >= MAX_DEBUG_MSG_SIZE )
         {
            writeOut( debugBuff, MAX_DEBUG_MSG_SIZE );
            return;
         }
      }
      va_list args;
      va_start(args, msg);
      retCode = vsnprintf(buff, MAX_DEBUG_MSG_SIZE, msg, args);
      va_end(args);

      if( retCode < 0 )
      {
         return; /* something is wrong. We cannot assert here as it will call this function. */
      }
      writtenBytes += retCode;
      buff += retCode;
      if( writtenBytes >= MAX_DEBUG_MSG_SIZE )
      {
         writeOut( debugBuff, MAX_DEBUG_MSG_SIZE );
         return;
      }

      retCode = snprintf(buff, MAX_DEBUG_MSG_SIZE, "\r\n");
      if( retCode < 0 )
      {
         return; /* something is wrong. We cannot assert here as it will call this function. */
      }
      writtenBytes += retCode;
      if( writtenBytes >= MAX_DEBUG_MSG_SIZE )
      {
         writeOut( debugBuff, MAX_DEBUG_MSG_SIZE );
         return;
      }
      writeOut( debugBuff, writtenBytes );
   }
}

/**
* \name     DEBUG_setSystemDebugLevel
* \brief    This is to set the system logging level
*
* \param    newLevel new verbose level
* \retval   None
*/
void DEBUG_setSystemLogLevel( DEBUG_verboseLevel_t newLevel )
{
   systemVerboseLevel = newLevel;
}

/**
* \name     writeOut
* \brief    Swrite the debug message out
*
* \param    newLevel new verbose level
* \param    newLevel new verbose level
* \retval   None
*/
static void writeOut( uint8_t *data, uint16_t size )
{
   UART_send( UART_DEBUG_PORT, data, size );
}

/**
* \name     assert_failed
* \brief    This function is used inside the STM32 libraries (We cannot change the naming to follow our standard coding).
*
* \param    file the file name the assert occured
* \param    line the line number the assert occured
* \retval   None
*/
void assert_failed(char *file, uint32_t line)
{
   DEBUG_logMsg( DEBUG_VERBOSE_LEVEL_CRITICAL, (uint8_t*)GET_FILE_NAME(file), line, "params_assert" );
}



