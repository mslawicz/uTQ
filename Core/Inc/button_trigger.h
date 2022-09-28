/*
 * button_trigger.h
 *
 *  Created on: 28 wrz 2022
 *      Author: marci
 */

#ifndef INC_BUTTON_TRIGGER_H_
#define INC_BUTTON_TRIGGER_H_

#include "stm32f4xx_hal.h"
#include "timer.h"
#include <functional>
#include <utility>

enum class ButTrigState : uint8_t
{
    Idle,
    Pressed,
    Released
};

class ButtonTrigger
{
public:
    ButtonTrigger(std::function<int8_t()> trigSource);      //function returning a button press request e.g. rotary encoder action
    void handler();
    std::pair<bool, bool> getButtonState() const { return std::make_pair(_buttonState1, _buttonState2); }
private:
    std::function<int8_t()> _trigSource;
    bool _buttonState1{false};
    bool _buttonState2{false};
    ButTrigState _state{ButTrigState::Idle};
    Timer _actionTimer;
    static constexpr uint32_t ActionTime = 30000;
};


#endif /* INC_BUTTON_TRIGGER_H_ */
