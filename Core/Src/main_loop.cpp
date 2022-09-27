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
#include "sh1106.h"
#include "fonts/fonts.h"
#include "rotary_encoder.h"
#include <sstream>  //XXX test

ADC_HandleTypeDef* pHadc;    //pointer to ADC object
SPI_HandleTypeDef* pHspi3;   //pointer to SPI3 object
uint16_t adcConvBuffer[MAX_ADC_CH]; //buffer for ADC conversion results
bool adcDataReady = true;
Display* pDisplay = nullptr;

#ifdef MONITOR
uint16_t mon_adc[MAX_ADC_CH];
GameControllerDataType mon_joy_data;
#endif

void mainLoop()
{
    constexpr uint32_t HeartbeatPeriod = 500000U;
    constexpr size_t AdcMedianFilterSize = 7;
    constexpr uint32_t AdcPeriod = GameController::ReportInterval / AdcMedianFilterSize;
    bool reverseOn = false;     //state of thrust reverser
    bool reverseOffArmed = false;    //automatic reverse off flag
    Timer statusLedTimer;
    Timer gameCtrlTimer;
    Timer adcTimer;
    Timer testTimer;    //XXX test
    testTimer.reset();

    LOG_ALWAYS("micro TQ v1.0");

    //assign system LEDs
    GPIO_TypeDef* heartbeatLedPort = LD2_GPIO_Port; //green LED
    uint16_t heartbeatLedPin = LD2_Pin;

    GameController gameController;  //USB link-to-PC object (class custom HID - joystick)

    pDisplay = new SH1106(pHspi3, DIS_CS_GPIO_Port, DIS_CS_Pin, DIS_DC_GPIO_Port, DIS_DC_Pin, DIS_RESET_GPIO_Port, DIS_RESET_Pin);     //OLED display
    RotaryEncoder encoder(ENC_CLK_GPIO_Port, ENC_CLK_Pin, ENC_DT_GPIO_Port, ENC_DT_Pin);  //rotary encoder object

    //ADC filter objects
    MedianFilter<uint16_t> throttleFilter(AdcMedianFilterSize);
    MedianFilter<uint16_t> propellerFilter(AdcMedianFilterSize);
    MedianFilter<uint16_t> mixtureFilter(AdcMedianFilterSize);

    Timer::start(pTimerHtim);

    /* main forever loop */
    while(true)
    {
        //process ADC conversions
        if(adcDataReady && adcTimer.hasElapsed(AdcPeriod))
        {
            adcDataReady = false;

#ifdef MONITOR
            memcpy(mon_adc, adcConvBuffer, MAX_ADC_CH);
#endif //MONITOR

            //filter ADC data
            throttleFilter.filter(Max12Bit - adcConvBuffer[AdcCh::throttle]);
            propellerFilter.filter(adcConvBuffer[AdcCh::propeller]);
            mixtureFilter.filter(adcConvBuffer[AdcCh::mixture]);

            /* request next conversions of analog channels */
            HAL_ADC_Start_DMA(pHadc, (uint32_t*)adcConvBuffer, pHadc->Init.NbrOfConversion);

            adcTimer.reset();
        }

        //process heartbeat LED
        if(statusLedTimer.hasElapsed(HeartbeatPeriod))
        {
            HAL_GPIO_TogglePin(heartbeatLedPort, heartbeatLedPin);
            statusLedTimer.reset();
        }

        //process thrust reverser button
        //activate reverser on button press when throttle is idle
        //deactivate reverser automatically when throttle is idle again
        if((reverseOn == false) &&      //reverser is off
           (HAL_GPIO_ReadPin(PB_RED_GPIO_Port, PB_RED_Pin) == GPIO_PinState::GPIO_PIN_RESET) &&       //reverse button is pressed
           (throttleFilter.getMedian() < ADC10Pct))     //throttle is < 10%
        {
            reverseOn = true;
            reverseOffArmed = false;
        }
        if((reverseOn == true) &&   //reverser is on
           (throttleFilter.getMedian() > ADC20Pct))     //throttle > 20%
        {
            reverseOffArmed = true;
        }
        if((reverseOffArmed == true) &&     //reverser auto off is armed
           (throttleFilter.getMedian() < ADC10Pct))     //throttle < 10%
        {
            reverseOn = false;
            reverseOffArmed = false;
        }

        //process USB reports
        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
        {
            //set game controller axes
            gameController.data.slider = scale<uint16_t, uint16_t>(0, Max12Bit, throttleFilter.getMedian(), 0, Max15Bit);
            gameController.data.dial = scale<uint16_t, uint16_t>(0, Max12Bit, propellerFilter.getMedian(), 0, Max15Bit);
            gameController.data.Z = scale<uint16_t, int16_t>(0, Max12Bit, mixtureFilter.getMedian(), -Max15Bit, Max15Bit);

            //set game controller buttons
            gameController.setButton(GameControllerButton::reverser, reverseOn);
            gameController.setButton(GameControllerButton::flapsUp, HAL_GPIO_ReadPin(FLAPS_UP_GPIO_Port, FLAPS_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::flapsDown, HAL_GPIO_ReadPin(FLAPS_DOWN_GPIO_Port, FLAPS_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearUp, HAL_GPIO_ReadPin(GEAR_UP_GPIO_Port, GEAR_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearDown, HAL_GPIO_ReadPin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::blueButton, HAL_GPIO_ReadPin(PB_BLUE_GPIO_Port, PB_BLUE_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::greenButton, HAL_GPIO_ReadPin(PB_GREEN_GPIO_Port, PB_GREEN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::leftToggle, HAL_GPIO_ReadPin(TOGGLE_LEFT_GPIO_Port, TOGGLE_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::rightToggle, HAL_GPIO_ReadPin(TOGGLE_RIGHT_GPIO_Port, TOGGLE_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);

            gameController.sendReport();
            gameCtrlTimer.reset();

#ifdef MONITOR
            mon_joy_data.slider = gameController.data.slider;
#endif  //MONITOR
        }

        //handle display actions
        if(pDisplay != nullptr)
        {
            pDisplay->handler();
        }

        //periodic rotary encoder calls
        encoder.handler();

        if(testTimer.hasElapsed(1000000))   //XXX test
        {
            pDisplay->putText(0, 0, "Arial 9", FontArial9);
            pDisplay->putText(0, 10, "Tahoma 11", FontTahoma11);
            pDisplay->putText(0, 21, "Tahoma 11b", FontTahoma11b);
            pDisplay->putText(0, 32, "Tahoma 14b", FontTahoma14b);
            pDisplay->putText(0, 46, "Tahoma 15", FontTahoma15);
            //pDisplay->putText(0, 32, "Tahoma 16b", FontTahoma16b);
            testTimer.reset();
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

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == pHspi3)
    {
        if(pDisplay != nullptr)
        {
            pDisplay->freeBus();
        }
    }
}
