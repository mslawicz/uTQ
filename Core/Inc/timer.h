/*
 * timer.h
 *
 *  Created on: May 21, 2022
 *      Author: marci
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f4xx_hal.h"

/* timer for counting time with 1 us resolution */
class Timer
{
public:
    static void start(TIM_HandleTypeDef* pHtim);
    void reset();
    uint32_t getElapsedTime() const;
    bool hasElapsed(uint32_t time) const;
private:
    inline static TIM_HandleTypeDef* _pHtim;
    uint32_t _refCntVal;    //reference counter value
};



#endif /* INC_TIMER_H_ */
