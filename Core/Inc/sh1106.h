/*
 * sh1106.h
 *
 *  Created on: 17 wrz 2022
 *      Author: marci
 */

#ifndef INC_SH1106_H_
#define INC_SH1106_H_

#include "stm32f4xx_hal.h"
#include "timer.h"
#include <vector>
#include <array>

enum class SH1106State : uint8_t
{
    start,
    resetOff,
    setUp,
    clearData,
    sendAddr,
    sendData
};

enum class SH1106Control : uint8_t
{
    command,
    data
};

class SH1106
{
public:
    SH1106(SPI_HandleTypeDef* pSPI,
           GPIO_TypeDef* csPort, uint16_t csPin,
           GPIO_TypeDef* dcPort, uint16_t dcPin,
           GPIO_TypeDef* resetPort, uint16_t resetPin);
    void handler();
    void write(SH1106Control controlPad, uint8_t* buffer, uint16_t length);
    void freeSPI();
    void putDot(uint8_t x, uint8_t y, bool inverse = false);
private:
    SH1106State _state{SH1106State::start};
    Timer _timer;
    SPI_HandleTypeDef* _pSPI;
    GPIO_TypeDef* _csPort;
    uint16_t _csPin;
    GPIO_TypeDef* _dcPort;
    uint16_t _dcPin;
    GPIO_TypeDef* _resetPort;
    uint16_t _resetPin;
    static constexpr uint32_t ResetTime = 15;
    const std::vector<uint8_t> SH1106InitData
    {
        0x32,   //DC voltage output value
        0x40,   //display line for COM0 = 0
        0x81,   //contrast
        0x80,
        0xA0,   //segment right rotation
        0xA4,   //display in normal mode
        0xA6,   //display normal indication
        0xAD,   //DC pump on
        0x8B,
        0xC0    //scan from 0 to N-1
    };
    bool _busy{false};       //indicate SPI is busy
    static constexpr size_t NoOfPages = 8;
    static constexpr size_t NoOfRows = 128;
    static constexpr size_t MaxX = NoOfRows;
    static constexpr size_t MaxY = 62;
    struct SH1106Page
    {
        uint8_t refreshFrom;
        uint8_t refreshTo;
        std::array<uint8_t, NoOfRows> buffer;
    };
    std::array<SH1106Page, NoOfPages> displayData;
    bool _refreshRequest{false};
    bool _displayOn{false};
    uint8_t _pageNo{0};
};


#endif /* INC_SH1106_H_ */
