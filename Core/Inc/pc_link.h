/*
 * pc_link.h
 *
 *  Created on: May 21, 2022
 *      Author: marci
 */

#ifndef INC_PC_LINK_H_
#define INC_PC_LINK_H_

#include "stm32f4xx_hal.h"
#include "usbd_custom_hid_if.h"

//#define GAME_CONTROLLER_TEST

struct GameControllerDataType
{
    int16_t X;
    int16_t Y;
    int16_t Z;          //mixture
    int16_t Rz;
    uint16_t Rx;
    uint16_t Ry;        //spoilers (air brakes)
    uint16_t slider;    //throttle
    uint16_t dial;      //propeller
    uint8_t hat;
    uint32_t buttons;
};

enum class GameControllerButton : uint8_t
{
    reverser,
    flapsUp,
    flapsDown,
    gearUp,
    gearDown,
    blueButton,
    greenButton,
    leftToggle,
    rightToggle,
    headingDec,
    headingInc,
    apHeading,
    altitudeDec,
    altitudeInc,
    vsDec,
    vsInc,
    apVs
};

class GameController
{
public:
    void sendReport();
    void setButton(GameControllerButton button, bool state);
    GameControllerDataType data;
    static constexpr uint32_t ReportInterval = 10000U;    //game controller report sending interval
private:
    void setTestData();
    uint8_t _testCounter{0};
};

#endif /* INC_PC_LINK_H_ */
