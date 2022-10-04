/*
 * display.h
 *
 *  Created on: 20 wrz 2022
 *      Author: marci
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f4xx_hal.h"
#include "fonts/fonts.h"
#include <string>

enum FontField
{
    FontSize = 0,
    FontWidth = 2,
    FontHeight = 3,
    FontStart = 4,
    FontCount = 5,
    FontWidthDef = 6
};

class Display
{
public:
    virtual ~Display() {}
    virtual void handler() = 0;
    virtual void freeBus() {}
    virtual void putDot(uint8_t x, uint8_t y, bool inverse = false) = 0;
    void putLine(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse = false);
    uint8_t putChar(uint8_t X, uint8_t Y, uint8_t ch, const uint8_t* font, bool inverse = false, uint8_t upToX = 0);
    uint8_t putText(uint8_t X, uint8_t Y, std::string text, const uint8_t* font, bool inverse = false, uint8_t upToX = 0);
    uint8_t getMaxX() const { return maxX; }
    bool isOn() const { return displayOn; }
    uint8_t putRectangle(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse = false);
    uint8_t putFrame(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse = false);
protected:
    size_t maxX{0};
    size_t maxY{0};
    bool displayOn{false};
private:
    uint8_t putChar2CharSpace(uint8_t X, uint8_t Y, const uint8_t* font, bool inverse = false, uint8_t upToX = 0);
};


#endif /* INC_DISPLAY_H_ */
