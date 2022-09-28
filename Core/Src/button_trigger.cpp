/*
 * button_trigger.cpp
 *
 *  Created on: 28 wrz 2022
 *      Author: marci
 */


#include "button_trigger.h"

ButtonTrigger::ButtonTrigger(std::function<int8_t()> trigSource) :
    _trigSource(trigSource)
{

}

void ButtonTrigger::handler()
{
    switch(_state)
    {
    case ButTrigState::Idle:
        //wait for a button press request
        {
            auto request = _trigSource();
            if(request != 0)
            {
                //one of the buttons must be pressed
                if(request == -1)
                {
                    _buttonState1 = true;
                }
                else if(request == 1)
                {
                    _buttonState2 = true;
                }
                _actionTimer.reset();
                _state = ButTrigState::Pressed;
            }
        }
        break;

    case ButTrigState::Pressed:
        if(_actionTimer.hasElapsed(ActionTime))
        {
            //button pressing time has elapsed
            _buttonState1 = _buttonState2 = false;
            _actionTimer.reset();
            _state = ButTrigState::Released;
        }
        break;

    case ButTrigState::Released:
        if(_actionTimer.hasElapsed(ActionTime))
        {
            //button released time has been elapsed
            _state = ButTrigState::Idle;
        }
        break;

    default:
        break;
    }
}
