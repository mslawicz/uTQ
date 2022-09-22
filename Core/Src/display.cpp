/*
 * display.cpp
 *
 *  Created on: 20 wrz 2022
 *      Author: marci
 */

#include "display.h"
#include "stdlib.h"

/*
 * draw any line from one point to another
 */
void Display::putLine(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse)
{
    if(abs(toX - fromX) >= abs(toY - fromY))
    {
        //horizontal distance is greater than vertical one - iterate horizontally
        int8_t dx = (toX >= fromX) ? 1 : -1;
        uint8_t x = fromX;
        while(x != toX)
        {
            uint8_t y = fromY + (x - fromX) * (toY - fromY) / (toX - fromX);
            putDot(x, y, inverse);
            x += dx;
        }
    }
    else
    {
        //vertical distance is greater than horizontal one - iterate vertically
        int8_t dy = (toY >= fromY) ? 1 : -1;
        uint8_t y = fromY;
        while(y != toY)
        {
            uint8_t x = fromX + (y- fromY) * (toX - fromX) / (toY - fromY);
            putDot(x, y, inverse);
            y += dy;
        }
    }
}

/*
 * displays character on the screen
 * ch - ascii code
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverse - clears pixels if true
 * upToX - if >0, stops at X==upToX
 */
uint8_t Display::putChar(uint8_t X, uint8_t Y,uint8_t ch, const uint8_t* font, bool inverse, uint8_t upToX)
{
    constexpr uint8_t BitsInByte = 8;
    enum FontField
    {
        FontSize = 0,
        FontWidth = 2,
        FontHeight = 3,
        FontStart = 4,
        FontCount = 5,
        FontWidthDef = 6
    };
    bool isSpace = false;

    if((ch < font[FontStart]) || (ch >= font[FontStart]+font[FontCount]))
    {
        // ascii code out of this font range
        return X;
    }

    // width of this char
    uint8_t charWidth = font[6 + ch - font[FontStart]];
    if(charWidth == 0)
    {
        isSpace = true;
        charWidth = font[FontWidth];
    }

    // height of this char
    uint8_t charHeight = font[FontHeight];

    // calculate index of this char definition in array
    uint16_t charDefinitionIndex = FontWidthDef + font[FontCount];
    for(uint8_t i = 0; i < ch - font[FontStart]; i++)
    {
        charDefinitionIndex += font[FontWidthDef + i] * (1 + (charHeight - 1) / BitsInByte);
    }

    // for every column
    uint8_t ix;
    for(ix = 0; ix < charWidth; ix++)
    {
        // if upToX!=0 then print up to this X limit
        if((upToX != 0) && (X+ix > upToX))
        {
            break;
        }
        // for every horizontal row
        for(uint8_t iy = 0; iy < charHeight; iy++)
        {
            uint8_t bitPattern = isSpace ? 0 : font[charDefinitionIndex + ix + (iy / BitsInByte) * charWidth];
            bool lastByte = iy / BitsInByte == charHeight / BitsInByte;
            uint8_t extraShift = lastByte ? BitsInByte - charHeight % BitsInByte : 0;
            putDot(X + ix, Y + iy, ((bitPattern >> (extraShift + iy % BitsInByte)) & 0x01) == inverse);
        }
    }

    return X + ix;
}
