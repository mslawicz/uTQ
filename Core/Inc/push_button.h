/*
 * push_button.h
 *
 *  Created on: 1 paź 2022
 *      Author: marci
 */

#ifndef INC_PUSH_BUTTON_H_
#define INC_PUSH_BUTTON_H_

#include "stm32f4xx_hal.h"
#include "timer.h"

class PushButton
{
public:
    PushButton(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState activeLevel = GPIO_PinState::GPIO_PIN_RESET);
    bool isPressed() { handler(); return _buttonLevel == _activeLevel; }
    bool isLongPressed() { handler(); return (_buttonLevel == _activeLevel) && (_stateTimer.hasElapsed(LongPressTime)); }
    bool hasBeenPressed();
private:
    enum class State : uint8_t
    {
        Stable,
        Debounce
    };
    void handler();
    GPIO_TypeDef* _port;
    uint16_t _pin;
    GPIO_PinState _activeLevel;     //push button active state
    State _state{State::Stable};    //state of the state machine
    GPIO_PinState _currentLevel;    //_current (last recorded) level
    Timer _stateTimer;
    static constexpr uint32_t DebounceTime = 20000;
    static constexpr uint32_t LongPressTime = 1000000;
    GPIO_PinState _buttonLevel;    //level recorded at the moment of press/release
    bool _hasBeenPressed{false};    //true if the button has been pressed since the last inquire
};

#endif /* INC_PUSH_BUTTON_H_ */
