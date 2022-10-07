/*
 * info_window.h
 *
 *  Created on: 3 paź 2022
 *      Author: marci
 */

#ifndef INC_INFO_WINDOW_H_
#define INC_INFO_WINDOW_H_

#include "stm32f4xx_hal.h"
#include "display.h"
#include "timer.h"

enum class InfoMode : uint8_t
{
    Timer,
    Reverser
};

struct InfoData
{
    InfoMode mode;
    bool timerResetRequest;
};

class InfoWindow
{
public:
    InfoWindow(Display* pDisplay);
    void handler(InfoData& infoData);
private:
    Display* _pDisplay;
    InfoMode _mode{InfoMode::Timer};
    InfoMode _previousMode{InfoMode::Reverser};
    static constexpr uint8_t FromX = 0;
    static constexpr uint8_t FromY = 16;
    static constexpr uint8_t ToX = 127;
    static constexpr uint8_t ToY = 46;
    uint8_t _secondsToDisplay{0};
    uint8_t _minutesToDisplay{0};
    uint32_t _elapsedTimeToDisplay{0};
    Timer _timer;   //pilot's timer
};


#endif /* INC_INFO_WINDOW_H_ */
