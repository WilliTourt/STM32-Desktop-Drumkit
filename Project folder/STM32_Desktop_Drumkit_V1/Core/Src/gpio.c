/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SIDESTICK_OUT_Pin|KICK_OUT_Pin|SNARE_OUT_Pin|HT_OUT_Pin
                          |MT_OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LT_OUT_Pin|OPENHIHAT_OUT_Pin|CLOSEHIHAT_OUT_Pin|CRASH_OUT_Pin
                          |LED_Pin|BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RIDE_OUT_Pin|PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SIDESTICK_OUT_Pin KICK_OUT_Pin SNARE_OUT_Pin HT_OUT_Pin
                           MT_OUT_Pin */
  GPIO_InitStruct.Pin = SIDESTICK_OUT_Pin|KICK_OUT_Pin|SNARE_OUT_Pin|HT_OUT_Pin
                          |MT_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LT_OUT_Pin OPENHIHAT_OUT_Pin CLOSEHIHAT_OUT_Pin CRASH_OUT_Pin
                           LED_Pin BUZZER_Pin */
  GPIO_InitStruct.Pin = LT_OUT_Pin|OPENHIHAT_OUT_Pin|CLOSEHIHAT_OUT_Pin|CRASH_OUT_Pin
                          |LED_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : RIDE_OUT_Pin */
  GPIO_InitStruct.Pin = RIDE_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RIDE_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PWR_EN_Pin */
  GPIO_InitStruct.Pin = PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_PRESS_Pin */
  GPIO_InitStruct.Pin = KEY_PRESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY_PRESS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_RDY_Pin */
  GPIO_InitStruct.Pin = USB_RDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_RDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CH345_ACK_IT_Pin */
  GPIO_InitStruct.Pin = CH345_ACK_IT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CH345_ACK_IT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
