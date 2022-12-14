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


/*
 * displays text on the screen
 * text - string to be displayed
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverse - clears pixels if true
 * upToX - if >0, prints the text up to X==upToX
 */
uint8_t Display::putText(uint8_t X, uint8_t Y, std::string text, const uint8_t* font, bool inverse, uint8_t upToX)
{
    for(size_t index = 0; index < text.size(); index++)
    {
        X = putChar(X, Y, text[index], font, inverse, upToX);
        if(index < text.size() - 1)
        {
            X = putChar2CharSpace(X, Y, font, inverse, upToX);
        }
    }

    while((upToX > 0) && (X < upToX) && (X < maxX))
    {
        X = putChar2CharSpace(X, Y, font, inverse, upToX);
    }

    return X;
}

/*
 * displays interspace on the screen
 * X,Y - upper left corner of character placement
 * font - font array from fonts.h
 * inverse - clears pixels if true
 * upToX - if >0, stops at X==upToX
 */
uint8_t Display::putChar2CharSpace(uint8_t X, uint8_t Y, const uint8_t* font, bool inverse, uint8_t upToX)
{
    // height of this space
    uint8_t charHeight = font[FontHeight];

    // width of this space
    uint8_t spaceWidth = 1 + (charHeight - 2) / 8;

    // for every column
    uint8_t ix;
    for(ix = 0; ix < spaceWidth; ix++)
    {
        // if upToX!=0 then print up to this X limit
        if((upToX != 0) && (X+ix > upToX))
        {
            break;
        }
        // for every horizontal row
        for(uint8_t iy = 0; iy < charHeight; iy++)
        {
            putDot(X + ix, Y + iy, !inverse);
        }
    }

    return X + ix;
}

/*
 * displays rectangle on the screen
 * fromX, fromY - upper left corner
 * toX, toY - lower right corner
 * inverse - clears pixels if true
 */
uint8_t Display::putRectangle(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse)
{
    uint8_t X;
    for(X = fromX; X <= toX; X++)
    {
        for(uint8_t Y = fromY; Y <= toY; Y++)
        {
            putDot(X, Y, inverse);
        }
    }

    return X;
}

/*
 * displays frame on the screen
 * fromX, fromY - upper left corner
 * toX, toY - lower right corner
 * inverse - clears pixels if true
 */
uint8_t Display::putFrame(uint8_t fromX, uint8_t fromY, uint8_t toX, uint8_t toY, bool inverse)
{
    for(uint8_t X = fromX + 1; X <= toX - 1; X++)
    {
        putDot(X, fromY, inverse);
        putDot(X, toY, inverse);
    }

    for(uint8_t Y = fromY; Y <= toY; Y++)
    {
        putDot(fromX, Y, inverse);
        putDot(toX, Y, inverse);
    }

    return toX;
}
