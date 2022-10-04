/*
 * info_window.cpp
 *
 *  Created on: 3 paź 2022
 *      Author: marci
 */

#include "info_window.h"

InfoWindow::InfoWindow(Display *pDisplay) :
    _pDisplay(pDisplay)
{
}

void InfoWindow::handler(InfoData& infoData)
{
    switch(infoData.mode)
    {
    case InfoMode::Reverser:
        if(infoData.mode != _previousMode)
        {
            static constexpr uint8_t TxtX = 15;
            static constexpr uint8_t TxtY = 23;
            _pDisplay->putRectangle(FromX, FromY, ToX, ToY, false);
            _pDisplay->putText(TxtX, TxtY, "REVERSE", FontTahoma20b, true);
        }
        break;

    case InfoMode::Timer:
        if(infoData.mode != _previousMode)
        {
            _pDisplay->putRectangle(FromX, FromY, ToX, ToY, true);

            static constexpr uint8_t MinutesX = 0;
            static constexpr uint8_t MinutesY = 22;
            static constexpr uint8_t MinutesSize = 38;
            static constexpr uint8_t BarLength = 59;
            static constexpr uint8_t BarHeight = 8;
            static constexpr uint8_t BarY = MinutesY + 2;
            static constexpr uint8_t FrameHight = BarHeight + 6;
            //XXX test of digits
            auto x = _pDisplay->putText(MinutesX, MinutesY, "28'", FontTahoma20b, false, MinutesX + MinutesSize);
            _pDisplay->putFrame(x, BarY, x + BarLength + 1, BarY + FrameHight - 1);
        }
        break;

    default:
        if(infoData.mode != _previousMode)
        {
            _pDisplay->putRectangle(FromX, FromY, ToX, ToY, true);
        }
        break;
    }

    _previousMode = infoData.mode;
}
