/*! \file fifo.c
*
*  \brief This module provides the function for FIFO buffer
*
*
*  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
*/

/*-------------------------------- Includes -----------------------------------*/
#include "fifo.h"

/*-------------------------------- Consts -------------------------------------*/

/*-------------------------------- Types --------------------------------------*/
typedef struct
{
   FIFO_handler_t handler;
   uint8_t data[0];
} fifoBaseElement_t;

/*-------------------------------- Macros -------------------------------------*/


/*-------------------------------- Variables ----------------------------------*/

/*---------------------------- Function Prototypes ----------------------------*/
static uint32_t getSize( void* fifoBuffer );
static uint32_t getFreeSize( void* fifoBuffer );

/*---------------------------- Function Definitions ---------------------------*/
/**
* \name     FIFO_init
* \brief    Initialize the FIFO module by initializing the external resources.
*
* \param    None
* \retval   None
*/
void FIFO_init( void )
{

}

/**
* \name     FIFO_initBuffer
* \brief    Clears a FIFO buffer and initializes its handler.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \param    size the size of the buffer
* \retval   None
*/
void FIFO_initBuffer( void* fifoBuffer, uint32_t size )
{
   ( (fifoBaseElement_t*) fifoBuffer)->handler.tail = 0;
   ( (fifoBaseElement_t*) fifoBuffer)->handler.head = 0;
   /* By design, FIFO module never let the buffer use the last byte as
   * it causes the head = tail happens when buffer is full.
   * This helps to consider, if Head == Tail, buffer is empty */
   ( (fifoBaseElement_t*) fifoBuffer)->handler.size = size + 1;
}

/**
* \name     FIFO_addData
* \brief    Adds data to the buffer.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \param    data to save into the buffer
* \param    size the size of the data
* \retval   Returns TRue if the operation is successful
*/
BOOL FIFO_addData( void* fifoBuffer, uint8_t* data, uint32_t size )
{
   uint32_t bufferFreeSize;

   bufferFreeSize = getFreeSize( fifoBuffer );

   if( bufferFreeSize < size )
   {
      return FALSE;
   }

   DISABLE_INTERRUPTS();

   if( ( (fifoBaseElement_t*) fifoBuffer)->handler.head < ( (fifoBaseElement_t*) fifoBuffer)->handler.tail )
   {
      memcpy( &( (fifoBaseElement_t*) fifoBuffer)->data[( (fifoBaseElement_t*) fifoBuffer)->handler.head], data, size );
      ( (fifoBaseElement_t*) fifoBuffer)->handler.head += size;
   }
   else
   {
      uint32_t data_to_write = MIN( size, ( (fifoBaseElement_t*) fifoBuffer)->handler.size - ( (fifoBaseElement_t*) fifoBuffer)->handler.head );
      memcpy( &( (fifoBaseElement_t*) fifoBuffer)->data[( (fifoBaseElement_t*) fifoBuffer)->handler.head], data, data_to_write );
      size -= data_to_write;
      if( size )
      {
         /* still data to be written */
         data += data_to_write;
         memcpy( &( (fifoBaseElement_t*) fifoBuffer)->data[0], data, size );
         ( (fifoBaseElement_t*) fifoBuffer)->handler.head = size;
      }
      else
      {
         ( (fifoBaseElement_t*) fifoBuffer)->handler.head += data_to_write;
      }
   }
   RESTORE_INTERRUPTS();
   return TRUE;
}

/**
* \name     FIFO_getData
* \brief    Reads data from FIFO buffer into the provided data pointer.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \param    data the buffer to save data to
* \param    size the size of the data
* \retval   Returns TRue if the operation is successful
*/
uint32_t FIFO_getData( void* fifoBuffer, uint8_t* data, uint32_t size )
{
   uint32_t total_bytes_to_read;

   DISABLE_INTERRUPTS();
   size = MIN( FIFO_getUsedSize( fifoBuffer ), size );
   total_bytes_to_read = size;
   if( total_bytes_to_read > 0 )
   {
      if( ( (fifoBaseElement_t*) fifoBuffer)->handler.tail < ( (fifoBaseElement_t*) fifoBuffer)->handler.head )
      {
         memcpy( data, &( (fifoBaseElement_t*) fifoBuffer)->data[( (fifoBaseElement_t*) fifoBuffer)->handler.tail], size );
         ( (fifoBaseElement_t*) fifoBuffer)->handler.tail += size;
      }
      else
      {
         uint32_t data_to_read = MIN( size, ( (fifoBaseElement_t*) fifoBuffer)->handler.size - ( (fifoBaseElement_t*) fifoBuffer)->handler.tail );
         memcpy( data, &( (fifoBaseElement_t*) fifoBuffer)->data[( (fifoBaseElement_t*) fifoBuffer)->handler.tail], data_to_read );
         size -= data_to_read;
         ( (fifoBaseElement_t*) fifoBuffer)->handler.tail += data_to_read;
         data += data_to_read;
         if( size )
         {
            /* still data to be read */
            memcpy( data, &( (fifoBaseElement_t*) fifoBuffer)->data[0], size );
            ( (fifoBaseElement_t*) fifoBuffer)->handler.tail = size;
         }
      }
   }

   RESTORE_INTERRUPTS();
   return total_bytes_to_read;
}

/**
* \name     FIFO_getUsedSize
* \brief    Returns the used buffer size in bytes.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \retval   Returns the used buffer size in bytes.
*/
uint32_t FIFO_getUsedSize( void* fifoBuffer )
{
   return( getSize( fifoBuffer ) - getFreeSize( fifoBuffer ) );
}

/**
* \name     getSize
* \brief    Returns buffer size in bytes.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \retval   Returns buffer size in bytes.
*/
static uint32_t getSize( void* fifoBuffer )
{
   /* By design, FIFO module never let the buffer use the last byte as
    * it causes the head = tail happens when buffer is full.
    * This helps to consider, if Head == Tail, buffer is empty */
   return ( (fifoBaseElement_t*) fifoBuffer)->handler.size - 1;
}

/**
* \name     getFreeSize
* \brief    Returns the free unused buffer size in bytes.
*
* \param    fifoBuffer the pointer to the fifo buffer
* \retval   Returns the free unused buffer size in bytes.
*/
static uint32_t getFreeSize( void* fifoBuffer )
{
   uint32_t freeSize;
   DISABLE_INTERRUPTS();
   if( ( (fifoBaseElement_t*) fifoBuffer)->handler.tail > ( (fifoBaseElement_t*) fifoBuffer)->handler.head )
   {
      freeSize = ( ( (fifoBaseElement_t*) fifoBuffer)->handler.tail - ( (fifoBaseElement_t*) fifoBuffer)->handler.head );
   }
   else
   {
      freeSize = ( ( (fifoBaseElement_t*) fifoBuffer)->handler.size - ( (fifoBaseElement_t*) fifoBuffer)->handler.head ) + ( (fifoBaseElement_t*) fifoBuffer)->handler.tail;
   }
   if( freeSize )
   {
      /* By design, FIFO module never let the buffer use the last byte as
       * it causes the head = tail happens when buffer is full.
       * This helps to consider, if Head == Tail, buffer is empty */
      freeSize--;
   }
   RESTORE_INTERRUPTS();
   return freeSize;
}

