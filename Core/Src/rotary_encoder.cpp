/*
 * rotary_encoder.cpp
 *
 *  Created on: 24 wrz 2022
 *      Author: marci
 */

#include "rotary_encoder.h"

RotaryEncoder::RotaryEncoder
(
        GPIO_TypeDef* clkPort, uint16_t clkPin,
        GPIO_TypeDef* dtPort, uint16_t dtPin,
        GPIO_TypeDef* pbPort, uint16_t pbPin
) :
    _clkPort(clkPort),
    _clkPin(clkPin),
    _dtPort(dtPort),
    _dtPin(dtPin),
    _pbPort(pbPort),
    _pbPin(pbPin)
{

}
