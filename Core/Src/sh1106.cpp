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
        write(SH1106Control::command, const_cast<uint8_t*>(SH1106InitData.data()), SH1106InitData.size());
        _state = SH1106State::clearData;
        break;

    case SH1106State::clearData:
        for(auto& page : displayData)
        {
            page.buffer.fill(0);    //clear page data
            page.refreshFrom = 0;
            page.refreshTo = NoOfRows - 1;
            _refreshRequest = true;     //display must be refreshed with cleared data
        }
        _state = SH1106State::refresh;
        break;

    case SH1106State::refresh:
        if(_busy)
        {
            break;
        }

        if(_refreshRequest)
        {
            bool allPagesRefreshed = true;
            for(auto& page : displayData)
            {
                if((page.refreshFrom < NoOfRows) &&
                   (page.refreshTo < NoOfRows) &&
                   (page.refreshFrom <= page.refreshTo))
                {
                    //this page must be refreshed
                    allPagesRefreshed = false;
                    write(SH1106Control::data, &page.buffer[page.refreshFrom], page.refreshTo - page.refreshFrom +1);
                    page.refreshFrom = page.refreshTo = NoOfRows;   //mark that this page is up to date
                    break;
                }
            }
            _refreshRequest = !allPagesRefreshed;
        }

        if(!_displayOn && !_refreshRequest)
        {
            //one-shot action after the first refresh
            uint8_t displayOnCmd = 0xAF;    //display on command code
            write(SH1106Control::command, &displayOnCmd, 1);
            _displayOn = true;
        }
        break;

    default:
        break;
    }
}

void SH1106::write(SH1106Control controlPad, uint8_t *buffer, uint16_t length)
{
    HAL_GPIO_WritePin(_dcPort, _dcPin, static_cast<GPIO_PinState>(controlPad));
    HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_RESET);      //CS active; to be released in interrupt
    if(HAL_SPI_Transmit_IT(_pSPI, buffer, length) == HAL_OK)
    {
        _busy = true;
    }
    else
    {
        HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_SET);      //release CS upon unsuccessful transmission
    }
}

void SH1106::freeSPI()
{
    HAL_GPIO_WritePin(_csPort, _csPin, GPIO_PinState::GPIO_PIN_SET);    //CS pin not active
    _busy = false;
}
