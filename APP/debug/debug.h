/*! \file debug.h
 *
 *  \brief Debug module for printing data out (all printf's)
 *
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__


#ifdef __cplusplus
extern "C" {
#endif
/********************************** Includes *******************************************/
#include "common.h"
#include "system.h"

/*********************************** Consts ********************************************/


/*********************************** Macros ********************************************/
#define DEBUG_log(MSG,...)               DEBUG_logMsg(DEBUG_VERBOSE_LEVEL_INFO,(uint8_t*)GET_FILE_NAME(__FILE__),__LINE__,MSG,##__VA_ARGS__)

/************************************ Types ********************************************/
typedef enum
{
   DEBUG_VERBOSE_LEVEL_CRITICAL,
   DEBUG_VERBOSE_LEVEL_WARNING,
   DEBUG_VERBOSE_LEVEL_INFO
} DEBUG_verboseLevel_t;

/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void DEBUG_pwrp();

void DEBUG_init();

void DEBUG_logMsg( DEBUG_verboseLevel_t logLevel, uint8_t* file, int32_t line, const char *msg, ... );

#ifdef __cplusplus
}
#endif
#endif /* __DEBUG_H__ */
