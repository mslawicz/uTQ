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
        GPIO_TypeDef* dtPort, uint16_t dtPin
) :
    _clkPort(clkPort),
    _clkPin(clkPin),
    _dtPort(dtPort),
    _dtPin(dtPin)
{

}

void RotaryEncoder::handler()
{
    GPIO_PinState currentClkValue = HAL_GPIO_ReadPin(_clkPort, _clkPin);

    switch(_state)
    {
    case RotEncState::Stable:
        if(currentClkValue != _lastClkValue)
        {
            //clk change after a stable period
            _newClkValue = currentClkValue;
            _newDtValue = HAL_GPIO_ReadPin(_dtPort, _dtPin);
            _stabilityTimer.reset();
            _state = RotEncState::Changed;
        }
        break;

    case RotEncState::Changed:
        if(currentClkValue != _lastClkValue)
        {
            //the clk pin is not stable yet
            _stabilityTimer.reset();
        }
        else
        {
            if(_stabilityTimer.hasElapsed(StabilityTime))
            {
                //clk pin is stable again
                if((currentClkValue == _newClkValue) && (currentClkValue == GPIO_PinState::GPIO_PIN_RESET))
                {
                    //the new stable clk value is 0 and it is different from the previous one
                    _pulseCounter += (_newDtValue == GPIO_PinState::GPIO_PIN_SET) ? 1 : -1;
                }
                _state = RotEncState::Stable;
            }
        }
        break;
    }

    _lastClkValue = currentClkValue;
}

//returns true if encoder is rotated to left and takes out one pulse from the queue
bool RotaryEncoder::isToLeft()
{
    bool retVal{false};
    if(_pulseCounter < 0)
    {
        retVal = true;
        _pulseCounter++;
    }
    return retVal;
}

//returns true if encoder is rotated to right and takes out one pulse from the queue
bool RotaryEncoder::isToRight()
{
    bool retVal{false};
    if(_pulseCounter > 0)
    {
        retVal = true;
        _pulseCounter--;
    }
    return retVal;
}
