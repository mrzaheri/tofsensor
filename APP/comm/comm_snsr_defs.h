/*! \file comm_snsr_defs.h
 *
 *  \brief Message definitions
 *
 *    Message definitions and variables are contained in this header
 *    file
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

#ifndef __COMM_SNSR_DEFS_H__
#define __COMM_SNSR_DEFS_H__

/********************************** Includes *******************************************/
#include "common.h"

/*********************************** Consts ********************************************/
#define COMM_SENS_MAX_PACKET_SIZE            (8u)

typedef enum
{
   /* common command ID for all sensors */
   COMM_SNSR_SELF_TEST_ID              = 0x01,

   /* common response ID for all sensors */
   COMM_SNSR_STATUS_RESP_ID            = 0x02,

   /* Range Sensor command IDs */
   COMM_SNSR_RANGE_SENSOR_DATA_ID      = 0x10,

   /* Max supported ID for commands: */
   COMM_SNSR_MAX_SUPPORTED_ID          = 0xFF
} COMM_SNSR_cmdId_t;


/******************************** Data Types ********************************************/
#pragma pack(1)
typedef struct
{
   uint8_t  msgID;                        /* Message ID                                */
   uint8_t  msgSize;                      /* Number of bytes in payload. Max 8         */
   uint8_t  morePackets;                  /* Number of additional packets to expect    */
} COMM_SNSR_msgHeader_t;

typedef struct
{
   uint16_t          distance;
   uint16_t          signalRate;
   uint8_t           error;
} COMM_SNSR_RANGE_data_t;

/**
 * @brief structure for unpacking sensor command packets
 */
typedef struct
{
   COMM_SNSR_msgHeader_t header;          //!< standard message header
   union
   {
      uint8_t bytes[COMM_SENS_MAX_PACKET_SIZE];

      /* Range Sensor Message */
      COMM_SNSR_RANGE_data_t              rangeData;

   } payload;
} COMM_SNSR_message_t;

#pragma pack(0)

#endif /* __COMM_SNSR_DEFS_H__ */
