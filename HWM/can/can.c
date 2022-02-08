/*! \file can.c
 *
 *  \brief intialize and common API for CAN transfer
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "can.h"


/*********************************** Consts ********************************************/
#define CAN_MORE_PACKETS            (8u)                                               /* EXT ID Offset of the 'More packets' byte                       */
#define CAN_STD_ID_OFFSET_16        (5u)                                               /* STD ID offset when used with 16 bit registers - left aligned   */
#define CAN_STD_ID_OFFSET_32        (18u)                                              /* STD ID offset when used with 32 bit registers - right aligned  */
#define CAN_MASK_STD_ID_32          (0x7FFuL << CAN_STD_ID_OFFSET_32)                  /* STD ID mask for 32 bit Ext ID register (right aligned)         */
#define CAN_MASK_STD_ID_16          (0x7FFuL << CAN_STD_ID_OFFSET_16)                  /* STD ID mask for 16 bit filter (upper 16 bits - left aligned)   */
#define RCP_FILTER_ID               (CAN_RCP_SRC_MSG_ID << CAN_STD_ID_OFFSET_16);      /* Filter value for filtering for messages from the RCP           */

#define MAX_RX_FIFO_BUFFERS     2

/************************************ Types ********************************************/

typedef struct
{
   CAN_HandleTypeDef hCAN;
   CAN_rxData_t dummyRx[MAX_RX_FIFO_BUFFERS];
   CAN_rxCallback_t rxCb;
   BOOL isInitialized;
   uint32_t deviceSpecificId;
} canHandler_t;

/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/
static canHandler_t handler[CAN_TOTAL_PORTS];

/****************************** Functions Prototype ************************************/
static CAN_indices_t getIndex( CAN_TypeDef* can );
static void irqHandler( CAN_TypeDef* can );
static void handleRxMessageNotification( CAN_HandleTypeDef* hcan,  uint32_t fifoIndex );

/****************************** Functions Definition ***********************************/
/**
* \name     CAN_pwrp
* \brief    Initialize the internal resources
*
* \param    None
* \retval   None
*/
void CAN_pwrp( void )
{
   memset( &handler, 0, sizeof( handler ) );

   handler[CAN_CMD_PORT].hCAN.Instance = CMD_CAN;
}

/**
* \name     CAN_init
* \brief    Initialize external resources
*
* \param    index the index of CAN defined in CAN_indices_t
* \param    deviceId CAN device ID
* \param    rxCallback the callback function on data receive
* \retval   None
*/
void CAN_init( CAN_indices_t index, uint16_t deviceId, CAN_rxCallback_t rxCallback )
{
   CAN_FilterTypeDef  sFilterConfig;
   HAL_StatusTypeDef retVal;

   ASSERT( handler[index].hCAN.Instance != NULL );

   if( index == CAN_CMD_PORT )
   {
      /* Set device specific ID. Lower 8 bits of the STD ID */
      handler[index].deviceSpecificId |= ( deviceId << CAN_STD_ID_OFFSET_32 );

      /* 1- Configure the CAN peripheral */
      handler[index].hCAN.Init.Prescaler              = 16;
      handler[index].hCAN.Init.Mode                   = CAN_MODE_NORMAL;
      handler[index].hCAN.Init.SyncJumpWidth          = CAN_SJW_1TQ;
      handler[index].hCAN.Init.TimeSeg1               = CAN_BS1_5TQ;
      handler[index].hCAN.Init.TimeSeg2               = CAN_BS2_4TQ;
      handler[index].hCAN.Init.TimeTriggeredMode      = DISABLE;
      handler[index].hCAN.Init.AutoBusOff             = ENABLE;
      handler[index].hCAN.Init.AutoWakeUp             = DISABLE;
      handler[index].hCAN.Init.AutoRetransmission     = DISABLE;
      handler[index].hCAN.Init.ReceiveFifoLocked      = DISABLE;
      handler[index].hCAN.Init.TransmitFifoPriority   = DISABLE;

      if( HAL_CAN_Init(&handler[index].hCAN) != HAL_OK )
      {
         DEBUG_LOG("CAN: Cannot initialize can index %d", index);
      }

      /* 2- Configure the CAN Filter */
      sFilterConfig.FilterIdHigh                      = RCP_FILTER_ID;           /* Filter for message from RCP only    */
      sFilterConfig.FilterIdLow                       = 0x0000;                  /* Ignore lower 18 bits - accept all   */
      sFilterConfig.FilterBank                        = 0;                       /* Use filter bank 0                   */
      sFilterConfig.FilterMode                        = CAN_FILTERMODE_IDMASK;   /* Filter using a mask                 */
      sFilterConfig.FilterScale                       = CAN_FILTERSCALE_32BIT;   /* Filter using a single 32 bit mask   */
      sFilterConfig.FilterMaskIdHigh                  = CAN_MASK_STD_ID_16;      /* Only examine STD ID bits for mask   */
      sFilterConfig.FilterMaskIdLow                   = 0x0000;                  /* Ignore lower 18 bits - do not mask  */
      sFilterConfig.FilterFIFOAssignment              = CAN_RX_FIFO0;            /* Assign filter to RX FIFO 0          */
      sFilterConfig.FilterActivation                  = CAN_FILTER_ENABLE;       /* Enable this filter                  */
      sFilterConfig.SlaveStartFilterBank              = 14u;                     /* Use first 14 filters for CAN 1      */

      if( HAL_CAN_ConfigFilter(&handler[index].hCAN, &sFilterConfig) != HAL_OK )
      {
         DEBUG_LOG("CAN: Cannot configure CAN filter for index %d", index);
      }

      handler[index].isInitialized = TRUE;

      retVal = HAL_CAN_ActivateNotification( &handler[index].hCAN, CAN_IER_FMPIE0 | CAN_IER_FMPIE1 ); /* Message pending in FIFO interrupt enabled*/
      retVal |= HAL_CAN_Start( &handler[index].hCAN );
      retVal |= HAL_CAN_WakeUp(&handler[index].hCAN );
      if( retVal != HAL_OK )
      {
         DEBUG_LOG("CAN: Cannot configure CAN filter for index %d", index);
      }
   }
}

/**
* \name     CAN_send
* \brief    This function adds the data of size into the FIFO of the specified CAN
*
* \param    index the index of CAN defined in CAN_indices_t
* \param    msg the poiter to message
* \retval   None
*/
void CAN_send( CAN_indices_t index, COMM_SNSR_message_t* msg )
{
   HAL_StatusTypeDef    retCode;
   CAN_TxHeaderTypeDef  frameHeader;
   uint32_t             txMailBox;

   ASSERT_STR(handler[index].isInitialized, "CAN handler not initialized");
   ASSERT(msg->header.msgSize <= CAN_MAX_DATA_LEN);

   frameHeader.StdId                = 0;                                            /* STD ID - not used                */
   frameHeader.ExtId                = msg->header.msgID & ~CAN_MASK_STD_ID_32;      /* Lower 8 bits for custom msg ID   */
   frameHeader.ExtId                |= msg->header.morePackets << CAN_MORE_PACKETS; /* Next 8 bits number extra packets */
   frameHeader.ExtId                |= handler[index].deviceSpecificId;             /* Upper 11 bits identifies source  */
   frameHeader.IDE                  = CAN_ID_EXT;                                   /* Use extended ID (29 bits)        */
   frameHeader.TransmitGlobalTime   = DISABLE;                                      /* Don't send the time stamp        */
   frameHeader.RTR                  = CAN_RTR_DATA;                                 /* Data frame                       */
   frameHeader.DLC                  = msg->header.msgSize;                          /* Size of data in frame - max 8    */

   retCode = HAL_CAN_AddTxMessage( &handler[index].hCAN, &frameHeader, msg->payload.bytes, &txMailBox );
   if( retCode != HAL_OK )
   {
      DEBUG_LOG("Cannot send can message. Error code: %d", retCode);
   }
}
/**
* \name     getIndex
* \brief    Find the can handler index using the instance
*
* \param    can the can instance
* \retval   CAN_indices_t the index of the can
*/
static CAN_indices_t getIndex( CAN_TypeDef* can )
{
   for( CAN_indices_t index = 0; index < CAN_TOTAL_PORTS; index++ )
   {
      if( handler[index].hCAN.Instance == can )
      {
         return index;
      }
   }
   return CAN_INVALID_INDEX;
}

/**
* \name     handleRxMessageNotification
* \brief    handles all Callback from peripheral on data pending on Rx
*
* \param    hcan instance of the can module peripheral
* \param    RxFifo Rx fifo index
* \retval   None
*/
static void handleRxMessageNotification( CAN_HandleTypeDef* hcan,  uint32_t fifoIndex )
{
   CAN_RxHeaderTypeDef header;
   CAN_indices_t index = getIndex( hcan->Instance );
   if( ( index == CAN_INVALID_INDEX ) || ( fifoIndex >= MAX_RX_FIFO_BUFFERS ) )
   {
      /* there is a problem */
      DEBUG_LOG("Cannot get data from fifo index %d", fifoIndex );
      return;
   }

   if( HAL_OK == HAL_CAN_GetRxMessage( hcan, fifoIndex, &header, handler[index].dummyRx[fifoIndex].data ) )
   {
      /* For our application, we consider the ID is standard ID */
      if( handler[index].rxCb != NULL )
      {
         handler[index].dummyRx[fifoIndex].dataSize = header.DLC;
         handler[index].dummyRx[fifoIndex].id = (uint8_t)(header.ExtId & 0xFFu);   /* Message ID is in lower 8 bits          */
         handler[index].dummyRx[fifoIndex].moreData = (uint8_t)((header.ExtId >> CAN_MORE_PACKETS) & 0xFFu);   /* 'More Packets' count in next 8 bits    */
         handler[index].rxCb( &handler[index].dummyRx[fifoIndex] );
      }
   }
}

/**
* \name     HAL_CAN_RxFifo0MsgPendingCallback
* \brief    Callback from peripheral on data pending on Rx0
*
* \param    can instance of the can module peripheral
* \retval   None
*/
void HAL_CAN_RxFifo0MsgPendingCallback( CAN_HandleTypeDef* hcan )
{
   handleRxMessageNotification( hcan,  CAN_RX_FIFO0 );
}

/**
* \name     HAL_CAN_RxFifo0MsgPendingCallback
* \brief    Callback from peripheral on data pending on Rx0
*
* \param    can instance of the can module peripheral
* \retval   None
*/
void HAL_CAN_RxFifo1MsgPendingCallback( CAN_HandleTypeDef* hcan )
{
   handleRxMessageNotification( hcan,  CAN_RX_FIFO1 );
}

/**
* \name     irqHandler
* \brief    This function handles the ISR for all can modules.
*
* \param    can instance of the can module peripheral
* \retval   None
*/
static void irqHandler( CAN_TypeDef* can )
{
   CAN_indices_t index = getIndex( can );
   if( index == CAN_INVALID_INDEX )
   {
      return;
   }
   HAL_CAN_IRQHandler( &handler[index].hCAN );
}

/**
* \name     CAN1_TX_IRQHandler
* \brief    TX on CA1 interrupt handler
*
* \param    None
* \retval   None
*/
void CAN1_TX_IRQHandler( void )
{
    irqHandler( CAN1 );
}

/**
* \name     CAN1_RX0_IRQHandler
* \brief    RX0 on CA1 interrupt handler
*
* \param    None
* \retval   None
*/
void CAN1_RX0_IRQHandler( void )
{
    irqHandler( CAN1 );
}

/**
* \name     CAN1_RX0_IRQHandler
* \brief    RX0 on CA1 interrupt handler
*
* \param    None
* \retval   None
*/
void CAN1_RX1_IRQHandler( void )
{
    irqHandler( CAN1 );
}
