/*
 * main_loop.cpp
 *
 *  Created on: May 19, 2022
 *      Author: marci
 */

#include "main.h"
#include "main_loop_api.h"
#include "main_loop.h"
#include "timer.h"
#include "pc_link.h"
#include "convert.h"
#include "constant.h"
#include "logger.h"
#include "monitor.h"
#include "median_filter.h"

ADC_HandleTypeDef* pHadc;    //pointer to ADC object
uint16_t adcConvBuffer[MAX_ADC_CH]; //buffer for ADC conversion results
bool adcDataReady = true;

#ifdef MONITOR
uint16_t mon_adc[MAX_ADC_CH];
#endif

void mainLoop()
{
    constexpr uint32_t HeartbeatPeriod = 500000U;
    constexpr size_t AdcMedianFilterSize = 7;
    constexpr uint32_t AdcPeriod = GameController::ReportInterval / AdcMedianFilterSize;
    Timer statusLedTimer;
    Timer gameCtrlTimer;
    Timer adcTimer;

    LOG_ALWAYS("uTQ v1.0");

    //assign system LEDs
    GPIO_TypeDef* heartbeatLedPort = LD2_GPIO_Port; //green LED
    uint16_t heartbeatLedPin = LD2_Pin;

    GameController gameController;  //USB link-to-PC object (class custom HID - joystick)

    //ADC filter objects
    MedianFilter<uint16_t> throttleFilter(AdcMedianFilterSize);

    Timer::start(pTimerHtim);

    /* main forever loop */
    while(true)
    {
        if(adcDataReady && adcTimer.hasElapsed(AdcPeriod))
        {
            adcDataReady = false;

#ifdef MONITOR
            memcpy(mon_adc, adcConvBuffer, MAX_ADC_CH);
#endif //MONITOR

            //filter ADC data
            throttleFilter.filter(adcConvBuffer[AdcCh::throttle]);

            /* request next conversions of analog channels */
            HAL_ADC_Start_DMA(pHadc, (uint32_t*)adcConvBuffer, pHadc->Init.NbrOfConversion);

            adcTimer.reset();
        }

        if(statusLedTimer.hasElapsed(HeartbeatPeriod))
        {
            HAL_GPIO_TogglePin(heartbeatLedPort, heartbeatLedPin);
            statusLedTimer.reset();
        }

        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
        {
            gameController.data.slider = scale<uint16_t, uint16_t>(0, Max12Bit, throttleFilter.getMedian(), 0, Max15Bit);

            gameController.sendReport();
            gameCtrlTimer.reset();
        }

    }
}


/**
  * @brief  Regular conversion complete callback in non blocking mode
  * @param  hadc pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc == pHadc)
    {
        adcDataReady = true;
    }
}

