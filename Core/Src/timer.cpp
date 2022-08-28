/*
 * timer.cpp
 *
 *  Created on: May 21, 2022
 *      Author: marci
 */

#include "timer.h"
#include "main_loop_api.h"

TIM_HandleTypeDef* pTimerHtim;  //pointer to htim object instance used by Timer class


void Timer::start(TIM_HandleTypeDef* pHtim)
{
    _pHtim = pHtim;
    HAL_TIM_Base_Start(_pHtim);
}

void Timer::reset()
{
    if(_pHtim != nullptr)
    {
        _refCntVal = _pHtim->Instance->CNT;
    }
}

uint32_t Timer::getElapsedTime() const
{
    if(_pHtim == nullptr)
    {
        return 0;
    }

    uint32_t cntValue =  _pHtim->Instance->CNT;
    return cntValue - _refCntVal;
}

bool Timer::hasElapsed(uint32_t time) const
{
    if(_pHtim == nullptr)
    {
        return false;
    }

    uint32_t cntValue =  _pHtim->Instance->CNT;
    return ((cntValue - _refCntVal) >= time);
}
