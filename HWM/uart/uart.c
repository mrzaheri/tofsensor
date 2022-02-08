/*! \file uart.c
 *
 *  \brief initialize and low level API for UART ports
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "uart.h"
#include "fifo.h"
#include "system.h"


/*********************************** Consts ********************************************/
#define DUMMY_RX_SIZE_MAX              1
#define DUMMY_TX_SIZE_MAX              8
#define TX_FIFO_SIZE                   256

#define UART_BAUD_RATE                 115200

/************************************ Types ********************************************/
FIFO_CREATE_TYPE( txFifo, TX_FIFO_SIZE )

typedef struct
{
   UART_HandleTypeDef uart;
   uint32_t  baudrate;
   uint8_t dummyRx[DUMMY_RX_SIZE_MAX];
   uint8_t dummyTx[DUMMY_TX_SIZE_MAX];
   UART_rxCallback_t rxCb;
   FIFO_ELEMENT_TYPE_txFifo *txFifo;
   BOOL isInitialized;
} uartHandler_t;

/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/
static uartHandler_t handler[UART_TOTAL_PORTS];
FIFO_ELEMENT_TYPE_txFifo txFifoElement[UART_TOTAL_PORTS];

/****************************** Functions Prototype ************************************/
static UART_indices_t getIndex( USART_TypeDef* uart );
static void irqHandler( USART_TypeDef* uart );

/****************************** Functions Definition ***********************************/
/**
* \name     UART_pwrp
* \brief    Initialize the internal resources
*
* \param    None
* \retval   None
*/
void UART_pwrp( void )
{
   memset( &handler, 0, sizeof( handler ) );
   handler[UART_DEBUG_PORT].uart.Instance = DEBUG_UART;

   for( UART_indices_t i = 0; i < UART_TOTAL_PORTS; i++ )
   {
      handler[i].txFifo = &txFifoElement[i];
   }
}

/**
* \name     UART_init
* \brief    Initialize external resources
*
* \param    index the index of UART defined in UART_indices_t
* \param    rxCallback the callback function on data receive
* \retval   None
*/
void UART_init( UART_indices_t index, UART_rxCallback_t rxCallback )
{

   if( handler[index].uart.Instance != NULL ) /* it is initialized in UART_pwrp */
   {
      handler[index].uart.Init.BaudRate = UART_BAUD_RATE;
      handler[index].uart.Init.WordLength = UART_WORDLENGTH_8B;
      handler[index].uart.Init.StopBits = UART_STOPBITS_1;
      handler[index].uart.Init.Parity = UART_PARITY_NONE;
      handler[index].uart.Init.Mode = UART_MODE_TX_RX;
      handler[index].uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
      handler[index].uart.Init.OverSampling = UART_OVERSAMPLING_16;
      handler[index].uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
      handler[index].uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

      if( HAL_UART_Init(&handler[index].uart) != HAL_OK )
      {
         DEBUG_LOG("UART: Cannot initialize uart index %d", index);
      }
      handler[index].rxCb = rxCallback;
      FIFO_initBuffer( handler[index].txFifo, TX_FIFO_SIZE );
      HAL_UART_Receive_IT( &handler[index].uart, handler[index].dummyRx, DUMMY_RX_SIZE_MAX );
      handler[index].isInitialized = TRUE;
   }
}

/**
* \name     UART_send
* \brief    This function adds the data of size into the FIFO of the specified UART
*
* \param    index the index of UART defined in UART_indices_t
* \param    data the poiter to data
* \param    size the size of bytes to send
* \retval   None
*/
void UART_send( UART_indices_t index, uint8_t *data, uint8_t size )
{
   if( handler[index].isInitialized )
   {
      if ( FIFO_addData( handler[index].txFifo, data, size ) == FALSE )
      {
         if( index != UART_DEBUG_PORT )
         {
            DEBUG_LOG("FIFO is full for uart %d", index );
         }
         return;
      }
      DISABLE_INTERRUPTS();
      if ( handler[index].uart.gState != HAL_UART_STATE_BUSY_TX )
      {
         /* Enable the UART Transmit Complete Interrupt as there is no ongoing process
          * This will cause a call to HAL_UART_TxCpltCallback, where the data will be sent out */
         SET_BIT( handler[index].uart.Instance->CR1, USART_CR1_TCIE );
      }
      RESTORE_INTERRUPTS();
   }
}

/**
* \name     getIndex
* \brief    Find the uart handler index using the instance
*
* \param    uart the uart instance
* \retval   UART_indices_t the index of the uart
*/
static UART_indices_t getIndex( USART_TypeDef* uart )
{
   for( UART_indices_t index = 0; index < UART_TOTAL_PORTS; index++ )
   {
      if( handler[index].uart.Instance == uart )
      {
         return index;
      }
   }
   return UART_INVALID_INDEX;
}

/**
* \name     irqHandler
* \brief    This function handles the ISR for all uart modules.
*
* \param    uart instance of the uart module peripheral
* \retval   None
*/
static void irqHandler( USART_TypeDef* uart )
{
   UART_indices_t index = getIndex( uart );
   if( index == UART_INVALID_INDEX )
   {
      return;
   }
   HAL_UART_IRQHandler( &handler[index].uart );
}

/**
* \name     HAL_UART_TxCpltCallback
* \brief    Tx Transfer completed callback from ST HAL drivers.
*
* \param    huart UART handle.
* \retval   None
*/
void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart )
{
   uint8_t bytesToSend;
   UART_indices_t index = getIndex( huart->Instance );

   if( index != UART_INVALID_INDEX )
   {
      /* check the fifo and send the next package if available */
      bytesToSend = FIFO_getData( handler[index].txFifo, handler[index].dummyTx, DUMMY_TX_SIZE_MAX );
      if( bytesToSend )
      {
         HAL_UART_Transmit_IT( &handler[index].uart, handler[index].dummyTx, bytesToSend );
      }
   }
}

/**
* \name     HAL_UART_RxCpltCallback
* \brief    Rx Receive completed callback from ST HAL drivers.
*
* \param    huart UART handle.
* \retval   None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   UART_indices_t index = getIndex( huart->Instance );
   if( index != UART_INVALID_INDEX )
   {
      if( handler[index].uart.ErrorCode == HAL_UART_ERROR_NONE )
      {
         if( handler[index].rxCb != NULL )
         {
            handler[index].rxCb( handler[index].dummyRx, DUMMY_RX_SIZE_MAX );
         }
      }
      HAL_UART_Receive_IT( &handler[index].uart, handler[index].dummyRx, DUMMY_RX_SIZE_MAX ); /* enable the interrupt again */
   }
}

/**
* \name     USART1_IRQHandler
* \brief    IRQ handler for USART1
*
* \param    None
* \retval   None
*/
void USART1_IRQHandler(void)
{
   irqHandler( USART1 );
}

/**
* \name     USART2_IRQHandler
* \brief    IRQ handler for USART2
*
* \param    None
* \retval   None
*/
void USART2_IRQHandler(void)
{
   irqHandler( USART2 );
}

/**
* \name     USART3_IRQHandler
* \brief    IRQ handler for USART3
*
* \param    None
* \retval   None
*/
void USART3_IRQHandler(void)
{
   irqHandler( USART3 );
}

/**
* \name     UART5_IRQHandler
* \brief    IRQ handler for UART5
*
* \param    None
* \retval   None
*/
void UART5_IRQHandler(void)
{
   irqHandler( USART3 );
}

