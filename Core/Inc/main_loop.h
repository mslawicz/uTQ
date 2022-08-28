/*
 * main_loop.h
 *
 *  Created on: May 19, 2022
 *      Author: marci
 */

#ifndef INC_MAIN_LOOP_H_
#define INC_MAIN_LOOP_H_

#include "stm32f4xx_hal.h"

enum AdcCh
{
    throttle,
    propeller,
    mixture,
    leftBrake,
    rightBrake
};

#endif /* INC_MAIN_LOOP_H_ */
