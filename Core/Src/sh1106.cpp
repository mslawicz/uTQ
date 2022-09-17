/*
 * sh1106.cpp
 *
 *  Created on: 17 wrz 2022
 *      Author: marci
 */

#include "sh1106.h"

SH1106::SH1106(GPIO_TypeDef* resetPort, uint16_t resetPin) :
    _resetPort(resetPort),
    _resetPin(resetPin)
{

}

void SH1106::handler()
{
    switch(_state)
    {
    case SH1106State::start:
        //reset display
        HAL_GPIO_WritePin(_resetPort, _resetPin, GPIO_PinState::GPIO_PIN_RESET);
        _timer.reset();
        _state = SH1106State::resetOff;
        break;

    case SH1106State::resetOff:
        if(_timer.hasElapsed(ResetTime))
        {
            //end of reset pulse
            HAL_GPIO_WritePin(_resetPort, _resetPin, GPIO_PinState::GPIO_PIN_SET);
            _state = SH1106State::setUp;
        }
        break;

    case SH1106State::setUp:
        break;

    default:
        break;
    }
}



