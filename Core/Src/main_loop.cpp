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
#include "median_filter.h"
#include "push_button.h"

#define VAR_MONITOR 1

IWDG_HandleTypeDef* pIwdg;       //pointer to watchdog handler
ADC_HandleTypeDef* pHadc;    //pointer to ADC object
uint16_t adcConvBuffer[MAX_ADC_CH]; //buffer for ADC conversion results
bool adcDataReady = true;
static float throttleFiltered = 0;
static float propellerFiltered = 0;
static float mixtureFiltered = 0;
#if VAR_MONITOR
int32_t g_mon[10];
#endif

void mainLoop()
{
    constexpr uint32_t HeartbeatPeriod = 500000U;
    constexpr uint32_t AdcPeriod = GameController::ReportInterval / 10;
    bool reverseOn = false;     //state of thrust reverser
    bool reverseOffArmed = false;    //automatic reverse off flag
    Timer statusLedTimer;
    Timer gameCtrlTimer;
    Timer adcTimer;

    HAL_IWDG_Refresh(pIwdg);
    Timer::start(pTimerHtim);
    LOG_ALWAYS("micro TQ v2.0");

    //assign system LEDs
    GPIO_TypeDef* heartbeatLedPort = LD2_GPIO_Port; //green LED
    uint16_t heartbeatLedPin = LD2_Pin;

    GameController gameController;  //USB link-to-PC object (class custom HID - joystick)

    //ADC filter objects
    MedianFilter<uint16_t> medianFilterThrottle;
    MedianFilter<uint16_t> medianFilterPropeller;
    MedianFilter<uint16_t> medianFilterMixture;

    Timer pilotsTimer;
    pilotsTimer.reset();

    /* main forever loop */
    while(true)
    {
        //process ADC conversions
        if(adcDataReady && adcTimer.hasElapsed(AdcPeriod))
        {
            adcDataReady = false;

            //filter ADC data
            medianFilterThrottle.perform(Max12Bit - adcConvBuffer[AdcCh::throttle]);
            throttleFiltered += AlphaEMA * (static_cast<float>(medianFilterThrottle.getMedian()) / Max12BitF - throttleFiltered);
            medianFilterPropeller.perform(adcConvBuffer[AdcCh::propeller]);
            propellerFiltered += AlphaEMA * (static_cast<float>(medianFilterPropeller.getMedian()) / Max12BitF - propellerFiltered);
            medianFilterMixture.perform(adcConvBuffer[AdcCh::mixture]);
            mixtureFiltered += AlphaEMA * (static_cast<float>(medianFilterMixture.getMedian()) / Max12BitF - mixtureFiltered);

            //optional monitoring
#if VAR_MONITOR
            g_mon[0] = Max12Bit - adcConvBuffer[throttle];
            g_mon[1] = adcConvBuffer[mixture];
            g_mon[2] = adcConvBuffer[propeller];

            g_mon[3] = throttleFiltered * 4096;
            g_mon[4] = mixtureFiltered * 4096;
            g_mon[5] = propellerFiltered * 4096;
#endif            

            /* request next conversions of analog channels */
            HAL_ADC_Start_DMA(pHadc, (uint32_t*)adcConvBuffer, pHadc->Init.NbrOfConversion);

            adcTimer.reset();
        }

        //process heartbeat LED
        if(statusLedTimer.hasElapsed(HeartbeatPeriod))
        {
            HAL_GPIO_TogglePin(heartbeatLedPort, heartbeatLedPin);
            statusLedTimer.reset();
            HAL_IWDG_Refresh(pIwdg);
        }

        //process thrust reverser button
        //activate reverser on button press when throttle is idle
        //deactivate reverser automatically when throttle is idle again
        if((reverseOn == false) &&      //reverser is off
           (HAL_GPIO_ReadPin(PB_RED_GPIO_Port, PB_RED_Pin) == GPIO_PinState::GPIO_PIN_RESET) &&       //reverse button is pressed
           (throttleFiltered < ADC10Pct))     //throttle is < 10%
        {
            reverseOn = true;
            reverseOffArmed = false;
        }
        if((reverseOn == true) &&   //reverser is on
           (throttleFiltered > ADC20Pct))     //throttle > 20%
        {
            reverseOffArmed = true;
        }
        if((reverseOffArmed == true) &&     //reverser auto off is armed
           (throttleFiltered < ADC10Pct))     //throttle < 10%
        {
            reverseOn = false;
            reverseOffArmed = false;
        }

        //process USB reports
        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
        {
            //set game controller axes
            gameController.data.slider = scale<float, uint16_t>(0, 1.0f, throttleFiltered, 0, Max15Bit);
            gameController.data.dial = scale<float, uint16_t>(0, 1.0f, propellerFiltered, 0, Max15Bit);
            gameController.data.Z = scale<float, int16_t>(0, 1.0f, mixtureFiltered, -Max15Bit, Max15Bit);

            //set game controller buttons
            gameController.data.buttons = 0;
            gameController.setButton(GameControllerButton::reverser, reverseOn);
            gameController.setButton(GameControllerButton::flapsUp, HAL_GPIO_ReadPin(FLAPS_UP_GPIO_Port, FLAPS_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::flapsDown, HAL_GPIO_ReadPin(FLAPS_DOWN_GPIO_Port, FLAPS_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearUp, HAL_GPIO_ReadPin(GEAR_UP_GPIO_Port, GEAR_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearDown, HAL_GPIO_ReadPin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::blueButton, HAL_GPIO_ReadPin(PB_BLUE_GPIO_Port, PB_BLUE_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::greenButton, HAL_GPIO_ReadPin(PB_GREEN_GPIO_Port, PB_GREEN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::leftToggle, HAL_GPIO_ReadPin(TOGGLE_LEFT_GPIO_Port, TOGGLE_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::rightToggle, HAL_GPIO_ReadPin(TOGGLE_RIGHT_GPIO_Port, TOGGLE_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);


            // unused data members
            gameController.data.X = 0;
            gameController.data.Y = 0;
            gameController.data.Rx = 0;
            gameController.data.Ry = 0;
            gameController.data.Rz = 0;
            gameController.data.hat = 0;

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

