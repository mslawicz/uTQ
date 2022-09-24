/*
 * rotary_encoder.h
 *
 *  Created on: 24 wrz 2022
 *      Author: marci
 */

#ifndef INC_ROTARY_ENCODER_H_
#define INC_ROTARY_ENCODER_H_

#include "stm32f4xx_hal.h"
#include "timer.h"

class RotaryEncoder
{
public:
    RotaryEncoder
    (
            GPIO_TypeDef* clkPort, uint16_t clkPin,
            GPIO_TypeDef* dtPort, uint16_t dtPin,
            GPIO_TypeDef* pbPort, uint16_t pbPin
    );
private:
    GPIO_TypeDef* _clkPort;
    uint16_t _clkPin;
    GPIO_TypeDef* _dtPort;
    uint16_t _dtPin;
    GPIO_TypeDef* _pbPort;
    uint16_t _pbPin;
};


#endif /* INC_ROTARY_ENCODER_H_ */
