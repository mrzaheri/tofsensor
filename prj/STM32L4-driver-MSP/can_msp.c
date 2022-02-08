/*******************************************************************************
* @file    can_msp.c
* @brief   Defines the project specific functions for HAL CAN driver.
*
*******************************************************************************/

/*-------------------------------- Includes -----------------------------------*/
#include "driver_msp_prj.h"

/*-------------------------------- Types --------------------------------------*/

/*-------------------------------- Macros -------------------------------------*/

/*-------------------------------- Variables ----------------------------------*/


/*---------------------------- Function Prototypes ----------------------------*/


/*---------------------------- Function Definitions ---------------------------*/
/****************************************************************
*
*   Name:    HAL_CAN_MspInit()
*
*   Brief:   Initialize the CAN MSP.
*
****************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
   GPIO_InitTypeDef  GPIO_InitStruct;

   if( hcan->Instance == CMD_CAN )
   {
      /* Enable GPIO clock */
      CMD_CAN_GPIO_CLK_ENABLE();

      /* Enable CAN clock */
      CMD_CAN_CLK_ENABLE();

      /* CAN GPIO pin configuration  */
      GPIO_InitStruct.Pin       = CMD_CAN_TX_GPIO_PIN;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_PULLUP;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = CMD_CAN_GPIO_AF;
      HAL_GPIO_Init(CMD_CAN_TX_GPIO_PORT, &GPIO_InitStruct);
      GPIO_InitStruct.Pin     = CMD_CAN_RX_GPIO_PIN;
      HAL_GPIO_Init(CMD_CAN_RX_GPIO_PORT, &GPIO_InitStruct);

      /* Interrupt for CAN */
      HAL_NVIC_SetPriority( CMD_CAN_IRQn, INTERRUPT_PRIORITY_HIGH, 0 );
      HAL_NVIC_EnableIRQ( CMD_CAN_IRQn );
   }
}

/****************************************************************
*
*   Name:    HAL_CAN_MspDeInit()
*
*   Brief:   DeInitialize the CAN MSP.
*
****************************************************************/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan)
{
   if( hcan->Instance == CMD_CAN )
   {
      CMD_CAN_FORCE_RESET();
      CMD_CAN_RELEASE_RESET();

      HAL_GPIO_DeInit(CMD_CAN_TX_GPIO_PORT, CMD_CAN_TX_GPIO_PIN);
      HAL_GPIO_DeInit(CMD_CAN_RX_GPIO_PORT, CMD_CAN_RX_GPIO_PIN);

      HAL_NVIC_DisableIRQ(CMD_CAN_IRQn);
   }
}

