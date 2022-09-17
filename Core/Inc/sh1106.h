/*
 * sh1106.h
 *
 *  Created on: 17 wrz 2022
 *      Author: marci
 */

#ifndef INC_SH1106_H_
#define INC_SH1106_H_

#include "stm32f4xx_hal.h"
#include "timer.h"

enum class SH1106State : uint8_t
{
    start,
    resetOff,
    setUp
};

class SH1106
{
public:
    SH1106(GPIO_TypeDef* resetPort, uint16_t resetPin);
    void handler();
private:
    SH1106State _state{SH1106State::start};
    Timer _timer;
    GPIO_TypeDef* _resetPort;
    uint16_t _resetPin;
    static constexpr uint32_t ResetTime = 15;
};


#endif /* INC_SH1106_H_ */
