/*! \file fifo.h
*
*  \brief Declares the FIFO module functions and variables
*
*
*  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
*/

#ifndef __FIFO_H__
#define __FIFO_H__

/*-------------------------------- Includes -----------------------------------*/
#include "common.h"

/*-------------------------------- Consts -------------------------------------*/


/*-------------------------------- Types --------------------------------------*/
typedef struct
{
   uint32_t size;
   uint32_t tail;
   uint32_t head;
} FIFO_handler_t;

/*-------------------------------- Macros -------------------------------------*/
/*
 * \brief The following, defines a structure (only) for a fifo element.
 *        Use it to create a FIFO structure with the NAME and SIZE of desired.
 * \note As the FIFO, by design, never let the buffer use the last byte
 *       The data variable size is defined with the "SIZE+1"
 * \note Wherever a fifo is generated, FIFO_buffer_init needs to be called to
 *       initialize the handle.
 */
#define FIFO_CREATE_TYPE(NAME, SIZE ) \
   typedef struct \
   { \
      FIFO_handler_t handler; \
      uint8_t data[(SIZE + 1)]; \
   } FIFO_ELEMENT_TYPE_##NAME; \

/* \brief The following, defines a structure and a static variable as fifo.
 *        Use it to create a FIFO structure with the NAME and SIZE of desired.
 * \note As the FIFO, by design, never let the buffer use the last byte
 *       The data variable size is defined with the "SIZE+1"
 * \note Wherever a fifo is generated, FIFO_buffer_init needs to be called to
 *       initialize the handle.
 */
#define FIFO_CREATE_ELEMENT(NAME, SIZE ) \
   FIFO_CREATE_TYPE(NAME, SIZE )\
   static FIFO_ELEMENT_TYPE_##NAME NAME;

/*-------------------------------- Variables ----------------------------------*/


/*---------------------------- Function Prototypes ----------------------------*/
void FIFO_init( void );

void FIFO_initBuffer( void* fifo_buffer, uint32_t size );

BOOL FIFO_addData( void* fifo_buffer, uint8_t* data, uint32_t size );

uint32_t FIFO_getData( void* fifo_buffer, uint8_t* data, uint32_t size );

uint32_t FIFO_getUsedSize( void* fifo_buffer );

#endif // __FIFO_H__
