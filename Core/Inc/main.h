/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define CUSTOM_HID_EPIN_SIZE 64
#define CUSTOM_HID_EPOUT_SIZE 64
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define HAT_LEFT_Pin GPIO_PIN_0
#define HAT_LEFT_GPIO_Port GPIOC
#define HAT_UP_Pin GPIO_PIN_1
#define HAT_UP_GPIO_Port GPIOC
#define HAT_DOWN_Pin GPIO_PIN_2
#define HAT_DOWN_GPIO_Port GPIOC
#define HAT_RIGHT_Pin GPIO_PIN_3
#define HAT_RIGHT_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define FLAPS_DOWN_Pin GPIO_PIN_4
#define FLAPS_DOWN_GPIO_Port GPIOC
#define DIS_DC_Pin GPIO_PIN_1
#define DIS_DC_GPIO_Port GPIOB
#define DIS_CS_Pin GPIO_PIN_2
#define DIS_CS_GPIO_Port GPIOB
#define PB_RED_Pin GPIO_PIN_10
#define PB_RED_GPIO_Port GPIOB
#define DIS_RESET_Pin GPIO_PIN_14
#define DIS_RESET_GPIO_Port GPIOB
#define GEAR_DOWN_Pin GPIO_PIN_7
#define GEAR_DOWN_GPIO_Port GPIOC
#define TEST_Pin GPIO_PIN_8
#define TEST_GPIO_Port GPIOC
#define PB_GREEN_Pin GPIO_PIN_8
#define PB_GREEN_GPIO_Port GPIOA
#define TOGGLE_RIGHT_Pin GPIO_PIN_9
#define TOGGLE_RIGHT_GPIO_Port GPIOA
#define FLAPS_UP_Pin GPIO_PIN_10
#define FLAPS_UP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define DIS_CSA15_Pin GPIO_PIN_15
#define DIS_CSA15_GPIO_Port GPIOA
#define HAT_SET_Pin GPIO_PIN_10
#define HAT_SET_GPIO_Port GPIOC
#define HAT_RST_Pin GPIO_PIN_11
#define HAT_RST_GPIO_Port GPIOC
#define HAT_MID_Pin GPIO_PIN_12
#define HAT_MID_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define TOGGLE_LEFT_Pin GPIO_PIN_4
#define TOGGLE_LEFT_GPIO_Port GPIOB
#define PB_BLUE_Pin GPIO_PIN_5
#define PB_BLUE_GPIO_Port GPIOB
#define GEAR_UP_Pin GPIO_PIN_6
#define GEAR_UP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
