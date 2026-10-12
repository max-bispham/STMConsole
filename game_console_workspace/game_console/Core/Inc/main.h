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
#include "stm32f1xx_hal.h"

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
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_1
#define OLED_CS_GPIO_Port GPIOA
#define BTN7_Pin GPIO_PIN_4
#define BTN7_GPIO_Port GPIOA
#define BTN6_Pin GPIO_PIN_5
#define BTN6_GPIO_Port GPIOA
#define BTN5_Pin GPIO_PIN_6
#define BTN5_GPIO_Port GPIOA
#define BTN4_Pin GPIO_PIN_7
#define BTN4_GPIO_Port GPIOA
#define BTN3_Pin GPIO_PIN_0
#define BTN3_GPIO_Port GPIOB
#define BTN2_Pin GPIO_PIN_1
#define BTN2_GPIO_Port GPIOB
#define BTN1_Pin GPIO_PIN_10
#define BTN1_GPIO_Port GPIOB
#define BTN0_Pin GPIO_PIN_11
#define BTN0_GPIO_Port GPIOB
#define SD_CS_Pin GPIO_PIN_8
#define SD_CS_GPIO_Port GPIOA
#define OLED_RES_Pin GPIO_PIN_6
#define OLED_RES_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_7
#define OLED_DC_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_8
#define LED_RED_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_9
#define LED_GREEN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
