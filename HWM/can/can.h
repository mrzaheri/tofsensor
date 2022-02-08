/*! \file can.h
 *
 *  \brief CAN module functions declerations
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __CAN_H__
#define __CAN_H__
/********************************** Includes *******************************************/
#include "common.h"
#include "comm_snsr_defs.h"

/*********************************** Consts ********************************************/
#define CAN_MAX_DATA_LEN         (8u)              /* Maximum number of data bytes in a CAN frame  */

#define CAN_RCP_SRC_MSG_ID       (0x700u)          /* Message from RCP                             */


/************************************ Types ********************************************/
typedef struct
{
   uint8_t data[CAN_MAX_DATA_LEN];
   uint32_t id;
   uint32_t dataSize;
   BOOL     moreData;
} CAN_rxData_t;

typedef void (*CAN_rxCallback_t)( CAN_rxData_t *data );

typedef enum
{
   CAN_CMD_PORT,

   CAN_TOTAL_PORTS,
   CAN_INVALID_INDEX = 0xFF
}CAN_indices_t;

typedef enum
{
   CAN_RX_FIFO_0 = 0,
   CAN_RX_FIFO_1,

   CAN_TOTAL_FIFOS         /* Keep always as the last one */
} CAN_fifo_t;

/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void CAN_pwrp( void );

void CAN_init( CAN_indices_t index, uint16_t deviceId, CAN_rxCallback_t rxCallback );

void CAN_send( CAN_indices_t index, COMM_SNSR_message_t* msg );

void CAN_emptyMailboxes( void );

BOOL CAN_GetRxMessage( CAN_indices_t handleIndex, CAN_fifo_t fifo, COMM_SNSR_message_t* msg);
#endif /* __CAN_H__ */
