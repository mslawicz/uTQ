/*
 * push_button.cpp
 *
 *  Created on: 1 paź 2022
 *      Author: marci
 */


#include "push_button.h"

PushButton::PushButton(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState activeLevel) :
    _port(port),
    _pin(pin),
    _activeLevel(activeLevel)
{
    _currentLevel = (_activeLevel == GPIO_PinState::GPIO_PIN_RESET) ? GPIO_PinState::GPIO_PIN_SET : GPIO_PinState::GPIO_PIN_RESET;
    _buttonLevel = _currentLevel;
}

void PushButton::handler()
{
    auto newLevel = HAL_GPIO_ReadPin(_port, _pin);
    switch(_state)
    {
    case State::Stable:
        if(newLevel != _currentLevel)
        {
            _currentLevel = newLevel;
            _buttonLevel = newLevel;
            if(_buttonLevel == _activeLevel)
            {
                _hasBeenPressed = true;
            }
            _stateTimer.reset();
            _state = State::Debounce;
        }
        break;

    case State::Debounce:
        if(newLevel != _currentLevel)
        {
            _currentLevel = newLevel;
            _stateTimer.reset();
        }

        if(_stateTimer.hasElapsed(DebounceTime))
        {
            _stateTimer.reset();
            _state = State::Stable;
        }
        break;

    default:
        break;
    }
}

bool PushButton::hasBeenPressed()
{
    auto hasBeenPressed = _hasBeenPressed;
    _hasBeenPressed = false;
    return hasBeenPressed;
}
