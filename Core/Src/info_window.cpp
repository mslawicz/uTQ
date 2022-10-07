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
    _timer.reset();
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
            static constexpr uint8_t MinutesX = 0;
            static constexpr uint8_t MinutesY = 22;
            static constexpr uint8_t MinutesSize = 35;
            static constexpr uint8_t FrameX = 41;
            static constexpr uint8_t FrameY = MinutesY + 2;
            static constexpr uint8_t FrameLength = 61;
            static constexpr uint8_t FrameHeight = 5;
            static constexpr uint8_t BarHeight = FrameHeight - 2;
            static constexpr uint32_t SecondToUs = 1000000U;
            bool displaySeconds{false};
            bool displayMinutes{false};
            static constexpr uint8_t MinuteToSeconds = 60U;
            static constexpr uint8_t MaxMinutes = 100U;
            if((infoData.mode != _previousMode) || infoData.timerResetRequest)
            {
                //mode has been changed or request for timer reset
                if(infoData.timerResetRequest)
                {
                    _timer.reset();
                    infoData.timerResetRequest = false;
                }
                //calculate minutes and seconds to display
                _elapsedTimeToDisplay = _timer.getElapsedTime();
                uint32_t fullSeconds = _elapsedTimeToDisplay / SecondToUs;
                _secondsToDisplay = fullSeconds % MinuteToSeconds;
                uint32_t fullMinutes = fullSeconds / MinuteToSeconds;
                _minutesToDisplay = fullMinutes % MaxMinutes;
                //draw all elements
                //first clear the whole area
                _pDisplay->putRectangle(FromX, FromY, ToX, ToY, true);
                //put seconds bar frame
                _pDisplay->putFrame(FrameX, FrameY, FrameX + FrameLength - 1, FrameY + FrameHeight - 1);
                static constexpr uint8_t NumbOfIntervals = 12;
                static constexpr uint8_t Interval = 60 / NumbOfIntervals;
                uint8_t dotRelPos;
                //put 5-second interval marks
                for(uint8_t i = 0; i <= NumbOfIntervals; i++)
                {
                    dotRelPos = 0;
                    _pDisplay->putDot(FrameX + i * Interval, FrameY + FrameHeight + dotRelPos++);
                    _pDisplay->putDot(FrameX + i * Interval, FrameY + FrameHeight + dotRelPos++);
                    static constexpr uint8_t BigIntervalCount = 3;
                    //draw longer mark every 15 seconds
                    if((i % BigIntervalCount) == 0)
                    {
                        _pDisplay->putDot(FrameX + i * Interval, FrameY + FrameHeight + dotRelPos++);
                        _pDisplay->putDot(FrameX + i * Interval, FrameY + FrameHeight + dotRelPos++);
                    }
                }
                //print 0, 30 and 60 second texts
                static constexpr int8_t RelPos0 = -2;
                static constexpr int8_t RelPos30 = 26;
                static constexpr int8_t RelPos60 = 56;
                _pDisplay->putText(FrameX + RelPos0, FrameY + FrameHeight + dotRelPos, "0", FontAlien7);
                _pDisplay->putText(FrameX + RelPos30, FrameY + FrameHeight + dotRelPos, "30", FontAlien7);
                _pDisplay->putText(FrameX + RelPos60, FrameY + FrameHeight + dotRelPos, "60", FontAlien7);
                //draw bar proportional to elapsed seconds
                _pDisplay->putRectangle(FrameX, FrameY + 1, FrameX + _secondsToDisplay, FrameY + BarHeight);
                displayMinutes = true;  //request printing of minutes
            }
            else
            {
                //another call in Timer mode (no need for redrawing all elements)
                if(_timer.getElapsedTime() - _elapsedTimeToDisplay >= SecondToUs)
                {
                    //one second elapsed
                    displaySeconds = true;
                    _secondsToDisplay++;
                    if(_secondsToDisplay >= MinuteToSeconds)
                    {
                        //one minute elapsed
                        displayMinutes = true;
                        _secondsToDisplay = 0;
                        _minutesToDisplay++;
                        if(_minutesToDisplay >= MaxMinutes)
                        {
                            _minutesToDisplay = 0;
                        }
                    }
                    _elapsedTimeToDisplay += SecondToUs;
                }
            }

            if(displayMinutes)
            {
                //request for printing minutes
                std::string minutesStr;
                static constexpr uint8_t MaxSingleDigit = 9;
                if(_minutesToDisplay <= MaxSingleDigit)
                {
                    minutesStr += " ";  //begin with a space for a single digit value
                }
                minutesStr += std::to_string(_minutesToDisplay) + "'";   //add minutes value and the minute symbol
                _pDisplay->putText(MinutesX, MinutesY, minutesStr, FontTahoma20b, false, MinutesX + MinutesSize);
            }

            if(displaySeconds)
            {
                //update the seconds bar
                if(_secondsToDisplay > 0)
                {
                    //make the bar longer with a new second
                    uint8_t dotRelPos = 1;
                    _pDisplay->putDot(FrameX + _secondsToDisplay, FrameY + dotRelPos++);
                    _pDisplay->putDot(FrameX + _secondsToDisplay, FrameY + dotRelPos++);
                    _pDisplay->putDot(FrameX + _secondsToDisplay, FrameY + dotRelPos++);
                }
                else
                {
                    //0 seconds - erase the bar
                    _pDisplay->putRectangle(FrameX + 1, FrameY + 1, FrameX + FrameLength - 2, FrameY + BarHeight, true);
                }
            }
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
