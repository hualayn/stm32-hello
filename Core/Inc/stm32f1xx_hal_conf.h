/**
  ******************************************************************************
  * @file    stm32f1xx_hal_conf.h
  * @brief   HAL configuration file for STM32F103C8T6.
  *
  *          Manually created (no CubeMX) with the minimal module set needed
  *          for the hello-world LED blink application:
  *            - HAL core (HAL_Init / SysTick tick)
  *            - RCC   (SystemClock_Config)
  *            - GPIO  (PC13 LED)
  *            - CORTEX
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* List of modules included by the user application handler -----------------*/
#define HAL_MODULE_ENABLED

/* #define HAL_ADC_MODULE_ENABLED   */
/* #define HAL_CAN_MODULE_ENABLED   */
/* #define HAL_CRC_MODULE_ENABLED   */
/* #define HAL_DAC_MODULE_ENABLED   */
/* #define HAL_DMA_MODULE_ENABLED   */
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
/* #define HAL_I2C_MODULE_ENABLED   */
/* #define HAL_I2S_MODULE_ENABLED   */
/* #define HAL_IWDG_MODULE_ENABLED  */
/* #define HAL_PCD_MODULE_ENABLED   */
/* #define HAL_PWR_MODULE_ENABLED   */
#define HAL_RCC_MODULE_ENABLED
/* #define HAL_RTC_MODULE_ENABLED   */
/* #define HAL_SPI_MODULE_ENABLED   */
/* #define HAL_TIM_MODULE_ENABLED   */
/* #define HAL_UART_MODULE_ENABLED  */
/* #define HAL_WWDG_MODULE_ENABLED  */

/* HAL CORTEX module (HAL_InitTick / SysTick) --------------------------------*/
#define HAL_CORTEX_MODULE_ENABLED

/* Include HAL modules -------------------------------------------------------*/
#if defined (HAL_RCC_MODULE_ENABLED)
  #include "stm32f1xx_hal_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#if defined (HAL_GPIO_MODULE_ENABLED)
  #include "stm32f1xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#if defined (HAL_DMA_MODULE_ENABLED)
  #include "stm32f1xx_hal_dma.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#if defined (HAL_CORTEX_MODULE_ENABLED)
  #include "stm32f1xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#if defined (HAL_FLASH_MODULE_ENABLED)
  #include "stm32f1xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

/* Common defines ------------------------------------------------------------*/
/* HSE value in Hz (Blue Pill crystal is 8 MHz) */
#define HSE_VALUE    ((uint32_t)8000000)
#define HSE_STARTUP_TIMEOUT ((uint32_t)100)

/* HSI value in Hz */
#define HSI_VALUE    ((uint32_t)8000000)
#define HSI_STARTUP_TIMEOUT ((uint32_t)100)

/* LSE value in Hz */
#define LSE_VALUE    ((uint32_t)32768)
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000)

/* LSI value in Hz */
#define LSI_VALUE    ((uint32_t)40000)

/* Time base configuration ---------------------------------------------------*/
#define  TICK_INT_PRIORITY ((uint32_t)0)
#define  USE_TICK_INT      ((TickPriorityType)TICK_INT_PRIORITY)

/* User can set this variable to 1 to enable the assert parameter check,
   in the HAL module */
#define  USE_ASSERT_PARAM  0

/* User can set this variable to 1 to enable the assert project check,
   in the HAL module */
#define  USE_ASSERT_FULL   0

/* Includes ------------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) if (!(expr)) assert_failed(__FILE__, __LINE__)
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
