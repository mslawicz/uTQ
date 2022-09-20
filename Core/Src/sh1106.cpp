/*
 * sh1106.cpp
 *
 *  Created on: 17 wrz 2022
 *      Author: marci
 */

#include "sh1106.h"

SH1106::SH1106(SPI_HandleTypeDef* pSPI,
               GPIO_TypeDef* csPort, uint16_t csPin,
               GPIO_TypeDef* dcPort, uint16_t dcPin,
               GPIO_TypeDef* resetPort, uint16_t resetPin) :
    _pSPI(pSPI),
    _csPort(csPort),
    _csPin(csPin),
    _dcPort(dcPort),
    _dcPin(dcPin),
    _resetPort(resetPort),
    _resetPin(resetPin)
{
    maxX = MaxX;
    maxY = MaxY;
}

void SH1106::handler()
{
    switch(_state)
    {
    case SH1106State::start:
        //reset display
        HAL_GPIO_WritePin(_resetPort, _resetPin, GPIO_PinState::GPIO_PIN_RESET);
        _timer.reset();
        _state = SH1106State::resetOff;
        break;

    case SH1106State::resetOff:
        if(_timer.hasElapsed(ResetTime))
        {
            //end of reset pulse
            HAL_GPIO_WritePin(_resetPort, _resetPin, GPIO_PinState::GPIO_PIN_SET);
            _state = SH1106State::setUp;
        }
        break;

    case SH1106State::setUp:
        //send control commands to the display chip
        write(SH1106Control::command, const_cast<uint8_t*>(SH1106InitData.data()), SH1106InitData.size());
        _state = SH1106State::clearData;
        break;

    case SH1106State::clearData:
        //clear data buffer and request refreshing
        for(auto& page : displayData)
        {
            page.buffer.fill(0);    //clear page data
            page.refreshFrom = 0;
            page.refreshTo = NoOfRows - 1;
            _refreshRequest = true;     //display must be refreshed with cleared data
            //XXX test
            page.buffer[120] = 0x0F;
        }
        _state = SH1106State::sendAddr;
        break;

    case SH1106State::sendAddr:
        if(_busy)
        {
            break;
        }

        if(_refreshRequest)
        {
            //find the first page that must be refreshed
            _pageNo = 0;
            while(_pageNo < NoOfPages)
            {
                if(displayData[_pageNo].refreshFrom <= displayData[_pageNo].refreshTo)
                {
                    //this page must be refreshed
                    uint8_t startColumn = displayData[_pageNo].refreshFrom + 2;
                    std::vector<uint8_t> addrData =     //display memory address
                    {
                            static_cast<uint8_t>(startColumn & 0x0F),  //lower column value
                            static_cast<uint8_t>(0x10 | ((startColumn >> 4)& 0x0F)),   //higher column value
                            static_cast<uint8_t>(0xB0 | _pageNo)    //page value
                    };
                    write(SH1106Control::command, addrData.data(), addrData.size());
                    _state = SH1106State::sendData;     //display data to be sent in the next transaction
                    break;
                }
                _pageNo++;
            }
            if(_pageNo >= NoOfPages)
            {
                //all pages refreshed - mark complete
                _refreshRequest = false;
            }
        }

        if(!_displayOn && !_refreshRequest)
        {
            //one-shot action after the first refresh
            uint8_t displayOnCmd = 0xAF;    //display on command code
            write(SH1106Control::command, &displayOnCmd, 1);
            _displayOn = true;
        }
        break;

    case SH1106State::sendData:
        if(_busy)
        {
            break;
        }

        //send display data to the display chip
        write(SH1106Control::data, &displayData[_pageNo].buffer[displayData[_pageNo].refreshFrom], displayData[_pageNo].refreshTo - displayData[_pageNo].refreshFrom +1);
        displayData[_pageNo].refreshFrom = NoOfRows;   //mark that this page is up to date
        displayData[_pageNo].refreshTo = 0;
        _state = SH1106State::sendAddr;
        break;

    default:
        break;
    }
}

void SH1106::write(SH1106Control controlPad, uint8_t *buffer, uint16_t length)
{
    _busy = true;
    HAL_GPIO_WritePin(_dcPort, _dcPin, static_cast<GPIO_PinState>(controlPad));
    HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_RESET);      //CS active; to be released in interrupt
    if(HAL_SPI_Transmit_IT(_pSPI, buffer, length) != HAL_OK)
    {
        HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_SET);      //release CS upon unsuccessful transmission
        _busy = false;
    }
}

void SH1106::freeBus()
{
    HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_SET);    //CS pin not active
    _busy = false;
}

void SH1106::putDot(uint8_t x, uint8_t y, bool inverse)
{
    if((x < maxX) && (y < maxY))
    {
        constexpr uint8_t PageHight = 8;
        uint8_t page = y / PageHight;
        uint8_t bitPos = y % PageHight;
        if(inverse)
        {
            displayData[page].buffer[x] &= ~(1 << bitPos);
        }
        else
        {
            displayData[page].buffer[x] |= (1 << bitPos);
        }
        if(x < displayData[page].refreshFrom)
        {
            displayData[page].refreshFrom = x;
            _refreshRequest = true;
        }
        if(x > displayData[page].refreshTo)
        {
            displayData[page].refreshTo = x;
            _refreshRequest = true;
        }
    }
}
