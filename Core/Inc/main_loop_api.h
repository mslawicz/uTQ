/*
 * main_loop_api.h
 *
 *  Created on: May 19, 2022
 *      Author: marci
 */

#ifndef INC_MAIN_LOOP_API_H_
#define INC_MAIN_LOOP_API_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"

#define MAX_ADC_CH  16
#define TRUE    1
#define FALSE   0

extern TIM_HandleTypeDef* pTimerHtim;  //pointer to htim object instance used by Timer class
extern ADC_HandleTypeDef* pHadc;    //pointer to ADC object
extern bool adcDataReady;
extern IWDG_HandleTypeDef* pIwdg;    //pointer to watchdog handler

#ifdef __cplusplus
extern "C"
{
#endif

void mainLoop();

#ifdef __cplusplus
}
#endif

#endif /* INC_MAIN_LOOP_API_H_ */
