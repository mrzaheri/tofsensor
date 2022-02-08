/*******************************************************************************
* @file    uart_msp.c
* @brief   Defines the project specific functions for HAL UART driver.
*
*******************************************************************************/

/*-------------------------------- Includes -----------------------------------*/
#include <driver_msp_prj.h>

/*-------------------------------- Types --------------------------------------*/

/*-------------------------------- Macros -------------------------------------*/

/*-------------------------------- Variables ----------------------------------*/


/*---------------------------- Function Prototypes ----------------------------*/


/*---------------------------- Function Definitions ---------------------------*/
/****************************************************************
*
*   Name:    HAL_UART_MspInit()
*
*   Brief:   Initialize the UART MSP.
*
****************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
   GPIO_InitTypeDef  GPIO_InitStruct;

   if( huart->Instance == DEBUG_UART )
   {
      /* Enable GPIO clock */
      DEBUG_UART_GPIO_CLK_ENABLE();

      /* Enable UART clock */
      DEBUG_UART_CLK_ENABLE();

      /* UART GPIO pin configuration  */
      GPIO_InitStruct.Pin       = DEBUG_UART_TX_GPIO_PIN;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_NOPULL;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = DEBUG_UART_GPIO_AF;
      HAL_GPIO_Init(DEBUG_UART_TX_GPIO_PORT, &GPIO_InitStruct);
      GPIO_InitStruct.Pin     = DEBUG_UART_RX_GPIO_PIN;
      HAL_GPIO_Init(DEBUG_UART_RX_GPIO_PORT, &GPIO_InitStruct);

      /* Interrupt for USART */
      HAL_NVIC_SetPriority(DEBUG_UART_IRQn, INTERRUPT_PRIORITY_HIGH, 0);
      HAL_NVIC_EnableIRQ(DEBUG_UART_IRQn);
   }
}

/****************************************************************
*
*   Name:    HAL_UART_MspDeInit()
*
*   Brief:   DeInitialize the UART MSP.
*
****************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
   if( huart->Instance == DEBUG_UART )
   {
      DEBUG_UART_FORCE_RESET();
      DEBUG_UART_RELEASE_RESET();

      HAL_GPIO_DeInit(DEBUG_UART_TX_GPIO_PORT, DEBUG_UART_TX_GPIO_PIN);
      HAL_GPIO_DeInit(DEBUG_UART_RX_GPIO_PORT, DEBUG_UART_RX_GPIO_PIN);

      HAL_NVIC_DisableIRQ(DEBUG_UART_IRQn);
   }
}

