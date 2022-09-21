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
