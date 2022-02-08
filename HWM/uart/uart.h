/*! \file uart.h
 *
 *  \brief UART module functions declerations
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __UART_H__
#define __UART_H__
/********************************** Includes *******************************************/
#include "common.h"

/*********************************** Consts ********************************************/


/************************************ Types ********************************************/
typedef void (*UART_rxCallback_t)( uint8_t *data, uint8_t size );

typedef enum
{
   UART_DEBUG_PORT,

   UART_TOTAL_PORTS,
   UART_INVALID_INDEX = 0xFF
}UART_indices_t;

/******************************* Global Variables **************************************/


/****************************** Functions Prototype ************************************/
void UART_pwrp( void );

void UART_init( UART_indices_t index, UART_rxCallback_t rx );

void UART_send( UART_indices_t index, uint8_t *data, uint8_t size );

#endif /* __UART_H__ */
