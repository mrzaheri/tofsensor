/*! \file comm.h
 *
 *  \brief Traffic controller for all inbound and outbound messages
 *
 *  All functions will act as a traffic controller in this file for all inbound and
 *  outbound messages. Messages will be queued. Some functions will be interrupt
 *  routine driven
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __COMM_H__
#define __COMM_H__

/********************************** Includes *******************************************/
#include "common.h"
#include "comm_snsr_defs.h"

/*********************************** Consts ********************************************/
#define MAX_MESSAGE_SIZE         8

/************************************ Types ********************************************/

/******************************* Global Variables **************************************/

/******************************** Local Variables **************************************/

/****************************** Functions Prototype ************************************/
void COMM_pwrp( void );

void COMM_init( void );

BOOL COMM_getMessage( COMM_SNSR_message_t* msg );

void COMM_send( COMM_SNSR_message_t* msg );

#endif /* __COMM_H__ */
