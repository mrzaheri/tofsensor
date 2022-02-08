/*! \file system.c
*
*  \brief System module defining of watchdog, delays, etc.
*
*
*  \author Mohammadreza Zaheri
 *  \copyright Copyright (c) 2020
*/


/********************************** Includes *******************************************/
#include "stm32l4xx_hal.h"

/********************************** config *********************************************/
#ifdef ENABLE_RANGE_SENSOR_APP
   #define SUPPORT_VL6180X             1
   #define SUPPORT_VL53L1              0
#endif

/* system config */
#define ENABLE_UART_COMM_STUFFING         0
#define TOTAL_STARTUP_BLINKS              1

/* GPIO clocks */
#define ENABLE_ALL_GPIO_CLOCKS()          __HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();

/* User LED */
#define USER_LED_PORT                     GPIOA
#define USER_LED_PIN                      GPIO_PIN_3

/* bin detect pin */
#define BIN_DETECT_PORT                   GPIOA
#define BIN_DETECT_PIN                    GPIO_PIN_4

/* RTC */
#define SENSOR_I2C                        I2C1
#define SENSOR_I2C_EV_IRQn                I2C1_EV_IRQn
#define SENSOR_I2C_CLK_ENABLE()           __HAL_RCC_I2C1_CLK_ENABLE()
#define SENSOR_I2C_CLK_DISABLE()          __HAL_RCC_I2C1_CLK_DISABLE()
#define SENSOR_I2C_FORCE_RESET()          __HAL_RCC_I2C1_FORCE_RESET()
#define SENSOR_I2C_RELEASE_RESET()        __HAL_RCC_I2C1_RELEASE_RESET()
#define SENSOR_I2C_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SENSOR_SCL_PORT                   GPIOB
#define SENSOR_SCL_PIN                    GPIO_PIN_6
#define SENSOR_SDA_PORT                   GPIOB
#define SENSOR_SDA_PIN                    GPIO_PIN_7
#define SENSOR_I2C_GPIO_AF                GPIO_AF4_I2C1

#if SUPPORT_VL6180X
   #define SENSOR_CE_PORT                    GPIOA
   #define SENSOR_CE_PIN                     GPIO_PIN_2
   #define SENSOR_INT_PORT                   GPIOA
   #define SENSOR_INT_PIN                    GPIO_PIN_1
   #define SENSOR_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#elif SUPPORT_VL53L1
   #define SENSOR_CE_PORT                    GPIOB
   #define SENSOR_CE_PIN                     GPIO_PIN_4
   #define SENSOR_INT_PORT                   GPIOB
   #define SENSOR_INT_PIN                    GPIO_PIN_3
   #define SENSOR_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#endif
#define SENSOR_INT_IRQn                   EXTI1_IRQn

/* debug UART*/
#define DEBUG_UART                       USART1
#define DEBUG_UART_CLK_ENABLE()          __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_UART_CLK_DISABLE()         __HAL_RCC_USART1_CLK_DISABLE()
#define DEBUG_UART_FORCE_RESET()         __HAL_RCC_USART1_FORCE_RESET()
#define DEBUG_UART_RELEASE_RESET()       __HAL_RCC_USART1_RELEASE_RESET()
#define DEBUG_UART_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define DEBUG_UART_TX_GPIO_PORT          GPIOA
#define DEBUG_UART_TX_GPIO_PIN           GPIO_PIN_9
#define DEBUG_UART_RX_GPIO_PORT          GPIOA
#define DEBUG_UART_RX_GPIO_PIN           GPIO_PIN_10
#define DEBUG_UART_GPIO_AF               GPIO_AF7_USART1

#define DEBUG_UART_IRQn                  USART1_IRQn

/* CAN ID GPIO pins */
#define CAN_ID0_GPIO_PORT          GPIOB
#define CAN_ID0_GPIO_PIN           GPIO_PIN_0
#define CAN_ID1_GPIO_PORT          GPIOA
#define CAN_ID1_GPIO_PIN           GPIO_PIN_7
#define CAN_ID2_GPIO_PORT          GPIOA
#define CAN_ID2_GPIO_PIN           GPIO_PIN_4

/* CAN bus definitions */
#define CMD_CAN                       CAN1
#define CMD_CAN_CLK_ENABLE()          __HAL_RCC_CAN1_CLK_ENABLE()
#define CMD_CAN_CLK_DISABLE()         __HAL_RCC_CAN1_CLK_DISABLE()
#define CMD_CAN_FORCE_RESET()         __HAL_RCC_CAN1_FORCE_RESET()
#define CMD_CAN_RELEASE_RESET()       __HAL_RCC_CAN1_RELEASE_RESET()
#define CMD_CAN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define CMD_CAN_TX_GPIO_PORT          GPIOA
#define CMD_CAN_TX_GPIO_PIN           GPIO_PIN_12
#define CMD_CAN_RX_GPIO_PORT          GPIOA
#define CMD_CAN_RX_GPIO_PIN           GPIO_PIN_11
#define CMD_CAN_GPIO_AF               GPIO_AF9_CAN1

#define CMD_CAN_IRQn                  CAN1_RX0_IRQn

/* Interrupts priority */
#define INTERRUPT_PRIORITY_HIGH        2
#define INTERRUPT_PRIORITY_MID         7
#define INTERRUPT_PRIORITY_LOW         15
/************************************ Macros ********************************************/

/* Disable/Restore Interrupts. Must be used in pair */
#define DISABLE_INTERRUPTS()  uint32_t prim;\
                              prim = __get_PRIMASK();\
                              __disable_irq();
#define RESTORE_INTERRUPTS()  if(!prim){__enable_irq();}

#define CLEAR_PIN(PORT,PIN)         HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
#define SET_PIN(PORT,PIN)           HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);

#define USER_LED_ON()               CLEAR_PIN(GPIOA, GPIO_PIN_3);
#define USER_LED_OFF()              SET_PIN(GPIOA, GPIO_PIN_3);

#define USER_LED_TOGGLE(X)          for(uint8_t i = 0; i < (X); i++ ){USER_LED_ON();DELAY_MSEC(500);USER_LED_OFF();DELAY_MSEC(500);}

/************************************ Types *********************************************/
