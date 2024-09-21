/*
 * constant.h
 *
 *  Created on: 31 maj 2022
 *      Author: marci
 */

#ifndef INC_CONSTANT_H_
#define INC_CONSTANT_H_

#include "stm32f4xx_hal.h"

constexpr uint16_t Max12Bit = 0x0FFF;
constexpr uint16_t Max14Bit = 0x3FFF;
constexpr uint16_t Max15Bit = 0x7FFF;
constexpr uint16_t Max16Bit = 0xFFFF;
constexpr float Max12BitF = 4095.0f;
constexpr float ADC10Pct = 0.1f;
constexpr float ADC20Pct = 0.2f;

constexpr float AlphaEMA = 0.01f;		//EMA filter smoothing factor

#endif /* INC_CONSTANT_H_ */
