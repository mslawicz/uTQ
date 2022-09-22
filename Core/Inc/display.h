/*
 * display.h
 *
 *  Created on: 20 wrz 2022
 *      Author: marci
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f4xx_hal.h"

class Display
{
public:
    virtual ~Display() {}
    virtual void handler() = 0;
    virtual void freeBus() {}
    virtual void putDot(uint8_t x, uint8_t y, bool inverse = false) = 0;
    void putLine(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse = false);
    uint8_t putChar(uint8_t X, uint8_t Y,uint8_t ch, const uint8_t* font, bool inverse = false, uint8_t upToX = 0);
protected:
    size_t maxX{0};
    size_t maxY{0};
};


#endif /* INC_DISPLAY_H_ */
