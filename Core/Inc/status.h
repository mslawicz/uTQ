/*
 * status.h
 *
 *  Created on: 4 paź 2022
 *      Author: marci
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "stm32f4xx_hal.h"
#include "display.h"
#include <string>
#include <vector>

enum class AircraftType : uint8_t
{
    Engine,
    Seaplane,
    Glider,
    Helicopter
};

struct AircraftTypeData
{
    AircraftType type;
    std::string text;
};

class Status
{
public:
    Status(Display* pDisplay);
    void handler();
    void changeAircraftType(uint8_t dIdx);
    AircraftType getAircraftType() const { return _aircraftTypes[_typeIdx].type; };
private:
    Display* _pDisplay;
    const std::vector<AircraftTypeData> _aircraftTypes
    {
        {AircraftType::Engine, "Eng"},
        {AircraftType::Seaplane, "Sea"},
        {AircraftType::Glider, "Gli"},
        {AircraftType::Helicopter, "Hel"}
    };
    uint8_t _typeIdx{0};
    uint8_t _previousTypeIdx{static_cast<uint8_t>(_aircraftTypes.size() - 1)};
    static constexpr uint8_t PosY = 0;
};

#endif /* STATUS_H_ */
