/*
 * pc_link.cpp
 *
 *  Created on: May 21, 2022
 *      Author: marci
 */


#include "pc_link.h"
#include <vector>

#ifdef MONITOR
#endif

void GameController::sendReport()
{
#ifdef GAME_CONTROLLER_TEST
    setTestData();
#endif

    std::vector<uint8_t> reportData
    {
        LOBYTE(data.X),
        HIBYTE(data.X),
        LOBYTE(data.Y),
        HIBYTE(data.Y),
        LOBYTE(data.Z),
        HIBYTE(data.Z),
        LOBYTE(data.Rz),
        HIBYTE(data.Rz),
        LOBYTE(data.Rx),
        HIBYTE(data.Rx),
        LOBYTE(data.Ry),
        HIBYTE(data.Ry),
        LOBYTE(data.slider),
        HIBYTE(data.slider),
        LOBYTE(data.dial),
        HIBYTE(data.dial),
        data.hat,
        static_cast<uint8_t>((data.buttons) & 0xFF),
        static_cast<uint8_t>((data.buttons >> 8) & 0xFF),
        static_cast<uint8_t>((data.buttons >> 16) & 0xFF),
        static_cast<uint8_t>((data.buttons >> 24) & 0xFF)
    };

    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, reportData.data(), reportData.size());    //send vector content
}

void GameController::setTestData()
{
    _testCounter++;
    data.X = data.Y = data.Z = data.Rz = _testCounter << 8; // @suppress("Avoid magic numbers")
    data.Rx = data.Ry = data.slider = data.dial = _testCounter << 7; // @suppress("Avoid magic numbers")
    data.hat = (_testCounter >> 5) + 1; // @suppress("Avoid magic numbers")
    uint8_t pattern = 1 << (_testCounter >> 5);
    data.buttons = pattern | (pattern << 8) | (pattern << 16) | (pattern << 24); // @suppress("Avoid magic numbers")
}

void GameController::setButton(GameControllerButton button, bool state)
{
    if(state)
    {
        data.buttons |= (1 << static_cast<uint8_t>(button));
    }
    else
    {
        data.buttons &= ~(1 << static_cast<uint8_t>(button));
    }
}
