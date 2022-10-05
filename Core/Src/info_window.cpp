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
        {
            static constexpr uint32_t SecondToUs = 1000000U;
            uint32_t fullSeconds = infoData.pTimer->getElapsedTime() / SecondToUs;
            static constexpr uint8_t MinuteToSeconds = 60U;
            uint8_t secondsToDisplay = fullSeconds % MinuteToSeconds;
            uint32_t fullMinutes = fullSeconds / MinuteToSeconds;
            static constexpr uint8_t MaxMinutes = 100U;
            uint8_t minutesToDisplay = fullMinutes % MaxMinutes;
            bool displaySeconds = (secondsToDisplay != _lastSecondsToDisplay);
            bool displayMinutes = (minutesToDisplay != _lastMinutesToDisplay);
            static constexpr uint8_t MinutesX = 0;
            static constexpr uint8_t MinutesY = 22;
            static constexpr uint8_t MinutesSize = 35;
            if(infoData.mode != _previousMode)
            {
                _pDisplay->putRectangle(FromX, FromY, ToX, ToY, true);

                static constexpr uint8_t FrameX = 41;
                static constexpr uint8_t FrameLength = 60;
                static constexpr uint8_t FrameHeight = 5;
                static constexpr uint8_t BarY = MinutesY + 2;
                _pDisplay->putFrame(FrameX, BarY, FrameX + FrameLength, BarY + FrameHeight);
                for(uint8_t i = 0; i <= 12; i++)
                {
                    _pDisplay->putDot(FrameX + i * 5, BarY + FrameHeight + 1);
                    _pDisplay->putDot(FrameX + i * 5, BarY + FrameHeight + 2);
                    if((i % 3) == 0)
                    {
                        _pDisplay->putDot(FrameX + i * 5, BarY + FrameHeight + 3);
                        _pDisplay->putDot(FrameX + i * 5, BarY + FrameHeight + 4);
                    }
                }
                _pDisplay->putText(FrameX - 2, BarY + FrameHeight + 5, "0", FontAlien7);
                _pDisplay->putText(FrameX + 26, BarY + FrameHeight + 5, "30", FontAlien7);
                _pDisplay->putText(FrameX + 56, BarY + FrameHeight + 5, "60", FontAlien7);
                displayMinutes = true;
            }

            if(displayMinutes)
            {
                std::string minutesStr;
                if(minutesToDisplay < 10)
                {
                    minutesStr += " ";
                }
                minutesStr += std::to_string(minutesToDisplay) + "'";
                _pDisplay->putText(MinutesX, MinutesY, minutesStr, FontTahoma20b, false, MinutesX + MinutesSize);
            }

            _lastSecondsToDisplay = secondsToDisplay;
            _lastMinutesToDisplay = minutesToDisplay;
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
