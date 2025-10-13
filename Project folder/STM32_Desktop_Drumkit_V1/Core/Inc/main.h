/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SIDESTICK_ADC_Pin GPIO_PIN_0
#define SIDESTICK_ADC_GPIO_Port GPIOC
#define KICK_ADC_Pin GPIO_PIN_1
#define KICK_ADC_GPIO_Port GPIOC
#define SNARE_ADC_Pin GPIO_PIN_2
#define SNARE_ADC_GPIO_Port GPIOC
#define HT_ADC_Pin GPIO_PIN_3
#define HT_ADC_GPIO_Port GPIOC
#define MT_ADC_Pin GPIO_PIN_0
#define MT_ADC_GPIO_Port GPIOA
#define LT_ADC_Pin GPIO_PIN_1
#define LT_ADC_GPIO_Port GPIOA
#define OPENHIHAT_ADC_Pin GPIO_PIN_4
#define OPENHIHAT_ADC_GPIO_Port GPIOA
#define CLOSEHIHAT_ADC_Pin GPIO_PIN_5
#define CLOSEHIHAT_ADC_GPIO_Port GPIOA
#define CRASH_ADC_Pin GPIO_PIN_6
#define CRASH_ADC_GPIO_Port GPIOA
#define RIDE_ADC_Pin GPIO_PIN_7
#define RIDE_ADC_GPIO_Port GPIOA
#define SIDESTICK_OUT_Pin GPIO_PIN_11
#define SIDESTICK_OUT_GPIO_Port GPIOB
#define KICK_OUT_Pin GPIO_PIN_12
#define KICK_OUT_GPIO_Port GPIOB
#define SNARE_OUT_Pin GPIO_PIN_13
#define SNARE_OUT_GPIO_Port GPIOB
#define HT_OUT_Pin GPIO_PIN_14
#define HT_OUT_GPIO_Port GPIOB
#define MT_OUT_Pin GPIO_PIN_15
#define MT_OUT_GPIO_Port GPIOB
#define LT_OUT_Pin GPIO_PIN_6
#define LT_OUT_GPIO_Port GPIOC
#define OPENHIHAT_OUT_Pin GPIO_PIN_7
#define OPENHIHAT_OUT_GPIO_Port GPIOC
#define CLOSEHIHAT_OUT_Pin GPIO_PIN_8
#define CLOSEHIHAT_OUT_GPIO_Port GPIOC
#define CRASH_OUT_Pin GPIO_PIN_9
#define CRASH_OUT_GPIO_Port GPIOC
#define RIDE_OUT_Pin GPIO_PIN_8
#define RIDE_OUT_GPIO_Port GPIOA
#define PWR_EN_Pin GPIO_PIN_15
#define PWR_EN_GPIO_Port GPIOA
#define KEY_PRESS_Pin GPIO_PIN_10
#define KEY_PRESS_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_11
#define LED_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOC
#define USB_RDY_Pin GPIO_PIN_3
#define USB_RDY_GPIO_Port GPIOB
#define CH345_ACK_IT_Pin GPIO_PIN_4
#define CH345_ACK_IT_GPIO_Port GPIOB
#define CH345_ACK_IT_EXTI_IRQn EXTI4_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
