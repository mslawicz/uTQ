/*
 * status.h
 *
 *  Created on: 4 paź 2022
 *      Author: marci
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "stm32f4xx_hal.h"
#include "display.h"

class Status
{
public:
    Status(Display* pDisplay);
    void handler();
private:
    Display* _pDisplay;
};

#endif /* STATUS_H_ */
