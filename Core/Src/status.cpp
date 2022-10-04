/*
 * status.cpp
 *
 *  Created on: 4 paź 2022
 *      Author: marci
 */

#include "status.h"

Status::Status(Display* pDisplay) :
    _pDisplay(pDisplay)
{
}

void Status::handler()
{
    if(_typeIdx != _previousTypeIdx)
    {
        //update status on display
        uint8_t x = 0;
        //x = _pDisplay->putChar(x, PosY, '&', FontTahoma14b);
        x = _pDisplay->putText(x, PosY, "&" + _aircraftTypes[_typeIdx].text, FontTahoma14b, false, _pDisplay->getMaxX());
    }

    _previousTypeIdx = _typeIdx;
}

void Status::changeAircraftType(uint8_t dIdx)
{
    _typeIdx = (_typeIdx + dIdx) % _aircraftTypes.size();
}
