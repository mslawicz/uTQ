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

    default:
        if(infoData.mode != _previousMode)
        {
            _pDisplay->putRectangle(FromX, FromY, ToX, ToY, true);
        }
        break;
    }

    _previousMode = infoData.mode;
}
