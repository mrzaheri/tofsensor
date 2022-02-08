/*! \file hwm.c
 *
 *  \brief Initializes the HW
 *
 *
 *  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
 */

/********************************** Includes *******************************************/
#include "hwm.h"
#include "system.h"

/*********************************** Consts ********************************************/
#define SYSCLK_PLL_M                    2
#define SYSCLK_PLL_N                    40

/* CAN Std ID values for identifying message sources. Use upper 3 bits of 11 bit STD ID */
#define CAN_RNG_SRC_MSG_ID       (0x100u)          /* Message from Range sensor                    */

/*********************************** Macros ********************************************/


/************************************ Types ********************************************/


/******************************* Global Variables **************************************/


/******************************** Local Variables **************************************/


/****************************** Functions Prototype ************************************/
static void configSystemClock(void);
static void setGpios( void );

/****************************** Functions Definition ***********************************/
/**
* \name     HWM_pwrp
* \brief    Low level system initialization
*
* \param    None
* \retval   None
*/
void HWM_pwrp( void )
{
   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

   configSystemClock();
   setGpios();

   TIMER_pwrp();
   UART_pwrp();
   I2C_pwrp();
   CAN_pwrp();
}

/**
* \name     HWM_init
* \brief    Initialize HWM
*
* \param    None
* \retval   None
*/
void HWM_init( void )
{
   TIMER_init();
   I2C_init();
}

/**
* \name     HWM_getCanId
* \brief    Reads the CAN ID from GPIO pins
*
* \param    None
* \retval   uint16_t representing the CAN ID
*/
uint16_t HWM_getCanId( void )
{
   uint16_t canID = CAN_RNG_SRC_MSG_ID;
   canID |= HAL_GPIO_ReadPin( CAN_ID0_GPIO_PORT, CAN_ID0_GPIO_PIN );
   canID |= ( HAL_GPIO_ReadPin( CAN_ID0_GPIO_PORT, CAN_ID1_GPIO_PIN ) << 1 );
   canID |= ( HAL_GPIO_ReadPin( CAN_ID0_GPIO_PORT, CAN_ID2_GPIO_PIN ) << 2 );
   return canID;
}

/**
* \name     configSystemClock
* \brief    Configure the system clock
*
* \param    None
* \retval   None
*/
static void configSystemClock(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

   HAL_PWR_EnableBkUpAccess();
   __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
   RCC_OscInitStruct.LSEState = RCC_LSE_ON;
   RCC_OscInitStruct.MSIState = RCC_MSI_ON;
   RCC_OscInitStruct.MSICalibrationValue = 0;
   RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
   RCC_OscInitStruct.PLL.PLLM = SYSCLK_PLL_M;
   RCC_OscInitStruct.PLL.PLLN = SYSCLK_PLL_N;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
   RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
   RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }

   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                 |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
   {
      Error_Handler();
   }
   PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
   PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
   PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
   {
      Error_Handler();
   }

   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
   {
      Error_Handler();
   }

   HAL_RCCEx_EnableMSIPLLMode();
}

/**
* \name     set_gpios
* \brief    setup GPIOs
*
* \param    None
* \retval   None
*/
static void setGpios( void )
{
   GPIO_InitTypeDef  GPIO_InitStruct;
   /* Enable all GPIOs clock */
   ENABLE_ALL_GPIO_CLOCKS();

   /* LEDs */
   GPIO_InitStruct.Pin       = USER_LED_PIN;
   GPIO_InitStruct.Pull      = GPIO_PULLUP;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
   GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
   HAL_GPIO_Init( USER_LED_PORT, &GPIO_InitStruct );
   USER_LED_OFF();

   /* CAN ID Gpios */
   GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pin       = CAN_ID0_GPIO_PIN;
   HAL_GPIO_Init( CAN_ID0_GPIO_PORT, &GPIO_InitStruct );
   GPIO_InitStruct.Pin       = CAN_ID1_GPIO_PIN;
   HAL_GPIO_Init( CAN_ID1_GPIO_PORT, &GPIO_InitStruct );
   GPIO_InitStruct.Pin       = CAN_ID2_GPIO_PIN;
   HAL_GPIO_Init( CAN_ID2_GPIO_PORT, &GPIO_InitStruct );
}

/**
* \name     EXTI1_IRQHandler
* \brief    LINE1 GPIO interrupt handler
*
* \param    None
* \retval   None
*/
void EXTI1_IRQHandler(void)
{
   HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
   HAL_NVIC_SetPriority( EXTI1_IRQn, 0, 0 );
   HAL_NVIC_EnableIRQ( EXTI1_IRQn );

   MAIN_signalEvent( MAIN_EVENT_SENSOR_DATA_READY );
}
