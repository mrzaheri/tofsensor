/*! \file comm.c
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

/********************************** Includes *******************************************/
#include "comm.h"
#include "hwm.h"
#include "main.h"
#include "comm_snsr_defs.h"

/*********************************** Consts ********************************************/

/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/


/****************************** Functions Prototype ************************************/


/****************************** Functions Definition ***********************************/
/**
* \name     COMM_pwrp
* \brief    power up the comm module and resources
*
* \param    None
* \retval   None
*/
void COMM_pwrp( void )
{

}

/**
* \name     COMM_init
* \brief    Initialize the message handler
*
* \param    None
* \retval   None
*/
void COMM_init( void )
{
   CAN_init( CAN_CMD_PORT, HWM_getCanId(), NULL );
}

/**
* \name     COMM_send
* \brief    Sends the message out on the CAN bus.
*
* \param    msg a pointer to the message
* \retval   None
*/
void COMM_send( COMM_SNSR_message_t* msg)
{
   ASSERT(msg->header.msgSize <= COMM_SENS_MAX_PACKET_SIZE);
   CAN_send( CAN_CMD_PORT, msg );
}

/**
* \name     COMM_getMessage
* \brief    Get the message package from the CAN FIFO
*
* \param    index CAN peripheral handler index
* \param    msg   Pointer to put message data structure to populate
* \retval   TRUE = message retrieved OK
*/
BOOL COMM_getMessage(COMM_SNSR_message_t* msg )
{
   return FALSE;
}
