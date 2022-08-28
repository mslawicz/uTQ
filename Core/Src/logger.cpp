/*
 * logger.cpp
 *
 *  Created on: 4 cze 2022
 *      Author: marci
 */


#include "logger.h"
#include "timer.h"
#include <vector>
#include <iomanip>
#include <set>

static Timer logTimer;
std::set<uint16_t> messageChecksums;

void logMessage(LogLevel level, std::ostream& ostr, bool once)
{
    const std::vector<std::string>levelText{"None", ":", "Error", "Warning", "Info", "Debug"};
    auto message = ((std::ostringstream&)ostr).str();
    if(once)
    {
        auto checksum = getChecksum(message);
        if(messageChecksums.find(checksum) == messageChecksums.end())
        {
            //the first display
            messageChecksums.insert(checksum);
        }
        else
        {
            //this message has been already displayed before
            return;
        }
    }
    auto levelValue = static_cast<uint8_t>(level);
    if(level <= currentLevel)
    {
        constexpr uint32_t UsInSec = 1000000U;
        constexpr uint8_t UsDigits = 6U;
        auto us = logTimer.getElapsedTime();
        std::cout << "\r[" << us / UsInSec << "." << std::setfill('0') << std::setw(UsDigits) << us % UsInSec << "] ";
        std::cout << levelText[levelValue] << ": ";
        std::cout << message << "\r\n";
        std::cout << ">" << std::flush;
    }
}

//calculate and return Fletcher checksum of a string
uint16_t getChecksum(std::string str)
{
    constexpr uint8_t _8bits = 8U;
    uint8_t cs1 = 0;
    uint8_t cs2 = 0;

    for (auto ch : str)
    {
        cs2 += cs1 += ch;
    }

    return ((cs2 << _8bits) + cs1);
}
