
/*
* Copyright (c) 2017, STMicroelectronics - All Rights Reserved
*
* This file is part of VL53L1 Core and is dual licensed,
* either 'STMicroelectronics
* Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, VL53L1 Core may be distributed under the terms of
* 'BSD 3-clause "New" or "Revised" License', in which case the following
* provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
********************************************************************************
*
*/


#include "vl53l1_platform.h"
#include "system.h"
#include "i2c.h"

#if defined(BUILD_WITH_FULL_API_ENABLED)
   #include "vl53l1_api.h"
   #define GET_I2C_ADDRESS(X)       (X->I2cDevAddr)
#else
   #include "vl53l1x_api.h"
   #define GET_I2C_ADDRESS(X)       (X)
#endif
#define VL53L1_MAX_I2C_XFER_SIZE   10 /* At present time it 6 byte max but that can change */
uint8_t i2cDataBuff[VL53L1_MAX_I2C_XFER_SIZE];

int VL53L1_I2CWrite(VL53L1_DEV dev, uint8_t  *buff, uint8_t len)
{
    if( TRUE == I2C_write(GET_I2C_ADDRESS(dev), buff, len) )
    {
        return 0;
    }
    return 1;
}

int VL53L1_I2CRead(VL53L1_DEV dev, uint8_t *buff, uint8_t len)
{
    if( TRUE == I2C_read(GET_I2C_ADDRESS(dev), buff, len) )
    {
        return 0;
    }
    return 1;
}

VL53L1_Error VL53L1_ReadMulti(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
    int status;
    uint8_t *buffer;

    buffer=i2cDataBuff;

    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL53L1_I2CWrite(Dev, (uint8_t *) buffer, (uint8_t)2);
    if( !status ){
        status=VL53L1_I2CRead(Dev, pdata, count);
    }
    return status;
}

VL53L1_Error VL53L1_WrByte(VL53L1_DEV dev, uint16_t index, uint8_t data)
{
   int  status;
   uint8_t *buffer;

   buffer=i2cDataBuff;
   buffer[0]=index>>8;
   buffer[1]=index&0xFF;
   buffer[2]=data;

   status=VL53L1_I2CWrite(dev, buffer,(uint8_t)3);
   return status;
}

VL53L1_Error VL53L1_WrWord(VL53L1_DEV dev, uint16_t index, uint16_t data)
{
   int  status;
   uint8_t *buffer;

   buffer=i2cDataBuff;
   buffer[0]=index>>8;
   buffer[1]=index&0xFF;
   buffer[2]=data>>8;
   buffer[3]=data&0xFF;

   status=VL53L1_I2CWrite(dev, buffer,(uint8_t)4);
   return status;
}

VL53L1_Error VL53L1_WrDWord(VL53L1_DEV dev, uint16_t index, uint32_t data)
{
   int  status;
   uint8_t *buffer;

   buffer=i2cDataBuff;
   buffer[0]=index>>8;
   buffer[1]=index&0xFF;
   buffer[2]=data>>24;
   buffer[3]=(data>>16)&0xFF;
   buffer[4]=(data>>8)&0xFF;;
   buffer[5]=data&0xFF;
   status=VL53L1_I2CWrite(dev, buffer,(uint8_t)6);
   return status;
}

VL53L1_Error VL53L1_RdByte(VL53L1_DEV dev, uint16_t index, uint8_t *data)
{
   int  status;
   uint8_t *buffer;

   buffer=i2cDataBuff;
   buffer[0]=index>>8;
   buffer[1]=index&0xFF;

   status=VL53L1_I2CWrite(dev, buffer, (uint8_t)2);
   if( !status ){
       status=VL53L1_I2CRead(dev, buffer,1);
       if( !status ){
           *data=buffer[0];
       }
   }

   return status;
}

VL53L1_Error VL53L1_RdWord(VL53L1_DEV dev, uint16_t index, uint16_t *data)
{
   int  status;
   uint8_t *buffer;

   buffer=i2cDataBuff;
   buffer[0]=index>>8;
   buffer[1]=index&0xFF;

   status=VL53L1_I2CWrite(dev, buffer, (uint8_t)2);
   if( !status){
       status=VL53L1_I2CRead(dev, buffer,2);
       if( !status ){
           /* VL6180x register are Big endian if cpu is be direct read direct into *data is possible */
           *data=((uint16_t)buffer[0]<<8)|(uint16_t)buffer[1];
       }
   }
   return status;
}

VL53L1_Error VL53L1_RdDWord(VL53L1_DEV dev, uint16_t index, uint32_t *data)
{
   int status;
   uint8_t *buffer;

   buffer=i2cDataBuff;

   buffer[0]=index>>8;
   buffer[1]=index&0xFF;

   status=VL53L1_I2CWrite(dev, (uint8_t *) buffer, (uint8_t)2);
   if( !status ){
       status=VL53L1_I2CRead(dev, buffer,4);
       if( !status ){
           /* VL6180x register are Big endian if cpu is be direct read direct into data is possible */
           *data=((uint32_t)buffer[0]<<24)|((uint32_t)buffer[1]<<16)|((uint32_t)buffer[2]<<8)|((uint32_t)buffer[3]);
       }
   }
   return status;
}

#if defined(BUILD_WITH_FULL_API_ENABLED)
VL53L1_Error VL53L1_WaitMs(VL53L1_DEV Dev __attribute__((unused)), int32_t wait_ms)
{
   VL53L1_Error status  = VL53L1_ERROR_NONE;
   SYSTEM_delayMS(wait_ms);
   return status;
}

VL53L1_Error VL53L1_WaitUs(VL53L1_DEV Dev __attribute__((unused)), int32_t wait_us)
{
   VL53L1_Error status  = VL53L1_ERROR_NONE;
   Delay_us(wait_us);
   return status;
}

VL53L1_Error VL53L1_WaitValueMaskEx(
	VL53L1_DEV    Dev,
	uint32_t      timeout_ms,
	uint16_t      index,
	uint8_t       value,
	uint8_t       mask,
	uint32_t      poll_delay_ms)
{
   uint8_t data;
   VL53L1_Error status;
   while (timeout_ms > 0)
   {
      status = VL53L1_RdByte(Dev, index, &data);
      if (status != VL53L1_ERROR_NONE)
      {
         return status;
      }
      if ((data & mask) == value)
      {
         return VL53L1_ERROR_NONE;
      }
      VL53L1_WaitMs(Dev, poll_delay_ms);
      timeout_ms -= MIN(poll_delay_ms, timeout_ms);
   }

   return VL53L1_ERROR_TIME_OUT;
}
#endif
