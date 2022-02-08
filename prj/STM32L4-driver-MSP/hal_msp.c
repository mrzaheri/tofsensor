/*******************************************************************************
* @file    hal_msp.c
* @brief   Defines the project specific functions for HAL driver.
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
*   Name:    HAL_MspInit()
*
*   Brief:   Initialize the HAL MSP.
*
****************************************************************/
void HAL_MspInit(void)
{
  __SYSCFG_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/****************************************************************
*
*   Name:    HAL_GPIO_EXTI_Callback()
*
*   Brief:   GPIO interrupt call back function
*
****************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}

