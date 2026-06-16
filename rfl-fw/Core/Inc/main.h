/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_crs.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void init();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define sx_nreset_Pin LL_GPIO_PIN_0
#define sx_nreset_GPIO_Port GPIOA
#define sx_busy_Pin LL_GPIO_PIN_1
#define sx_busy_GPIO_Port GPIOA
#define sx_dio1_Pin LL_GPIO_PIN_2
#define sx_dio1_GPIO_Port GPIOA
#define sx_dio3_Pin LL_GPIO_PIN_3
#define sx_dio3_GPIO_Port GPIOA
#define sx_dio2_Pin LL_GPIO_PIN_4
#define sx_dio2_GPIO_Port GPIOA
#define sx_cs_Pin LL_GPIO_PIN_0
#define sx_cs_GPIO_Port GPIOB
#define led2_Pin LL_GPIO_PIN_1
#define led2_GPIO_Port GPIOB
#define led1_Pin LL_GPIO_PIN_15
#define led1_GPIO_Port GPIOB
#define sky_antsel_Pin LL_GPIO_PIN_8
#define sky_antsel_GPIO_Port GPIOA
#define sky_csd_Pin LL_GPIO_PIN_9
#define sky_csd_GPIO_Port GPIOA
#define sky_cps_Pin LL_GPIO_PIN_10
#define sky_cps_GPIO_Port GPIOA
#define d4_Pin LL_GPIO_PIN_0
#define d4_GPIO_Port GPIOD
#define sky_ctx_Pin LL_GPIO_PIN_3
#define sky_ctx_GPIO_Port GPIOD
#define d3_Pin LL_GPIO_PIN_6
#define d3_GPIO_Port GPIOB
#define d2_Pin LL_GPIO_PIN_7
#define d2_GPIO_Port GPIOB
#define d1_Pin LL_GPIO_PIN_8
#define d1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
