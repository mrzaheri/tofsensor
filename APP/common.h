/*! \file board.h
*
*  \brief Common project include file
*
*
*  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
*/


/********************************** Includes *******************************************/
#include <stdio.h>
#include "stdint.h"
#include "string.h"
#include "board.h"
#include "main.h"
#include "system.h"
#include "debug.h"


/*********************************** Consts ********************************************/
#ifndef NULL
   #define NULL            0
#endif
#ifndef FALSE
   #define FALSE           0
#endif
#ifndef TRUE
   #define TRUE            1
#endif

#define BOOL               uint8_t



#define PARAMETER_NOT_USED(p) ((void) (p))
/************************************ Types ********************************************/


/************************************ Macros *******************************************/
#define DELAY_MSEC(X)       SYSTEM_delayMS(X)

#define MIN(X,Y)           ((X)>(Y))?(Y):(X)
#define MAX(X,Y)           ((X)<(Y))?(Y):(X)

#define GET_FILE_NAME(FILE)             (strrchr((char *)FILE, '/') ? (uint8_t*)(strrchr((char *)FILE, '/') + 1):(uint8_t*)(FILE))

#define DEBUG_LOG(MSG,...)              DEBUG_logMsg(DEBUG_VERBOSE_LEVEL_INFO,(uint8_t*)GET_FILE_NAME(__FILE__),__LINE__, MSG,##__VA_ARGS__ )

#define Error_Handler()                 DEBUG_logMsg(DEBUG_VERBOSE_LEVEL_CRITICAL,(uint8_t*)GET_FILE_NAME(__FILE__),__LINE__, "Error Handler Call" )


#ifdef DEBUG
   #define ASSERT(cond)                if (!(cond)) {while(1);}
   #define ASSERT_STR(cond, MSG,...)   if (!(cond)) \
                                       {\
                                          DEBUG_logMsg(DEBUG_VERBOSE_LEVEL_CRITICAL,(uint8_t*)GET_FILE_NAME(__FILE__),__LINE__, MSG,##__VA_ARGS__);\
                                          while(1);\
                                       }
#else
   #define ASSERT(cond)
   #define ASSERT_STR(cond, MSG,...)
#endif

/************************************ Types *********************************************/
