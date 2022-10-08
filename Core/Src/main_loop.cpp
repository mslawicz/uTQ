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
#include "menu.h"
#include "push_button.h"
#include "info_window.h"
#include "status.h"

ADC_HandleTypeDef* pHadc;    //pointer to ADC object
SPI_HandleTypeDef* pHspi2;   //pointer to SPI2 object
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
    uint8_t lastMenuItemIdx = 0xFF; //remembers the previous menu item index
    Timer statusLedTimer;
    Timer gameCtrlTimer;
    Timer adcTimer;

    Timer::start(pTimerHtim);
    LOG_ALWAYS("micro TQ v1.0");

    //assign system LEDs
    GPIO_TypeDef* heartbeatLedPort = LD2_GPIO_Port; //green LED
    uint16_t heartbeatLedPin = LD2_Pin;

    GameController gameController;  //USB link-to-PC object (class custom HID - joystick)

    pDisplay = new SH1106(pHspi2, DIS_CS_GPIO_Port, DIS_CS_Pin, DIS_DC_GPIO_Port, DIS_DC_Pin, DIS_RESET_GPIO_Port, DIS_RESET_Pin);     //OLED display

    //ADC filter objects
    MedianFilter<uint16_t> throttleFilter(AdcMedianFilterSize);
    MedianFilter<uint16_t> propellerFilter(AdcMedianFilterSize);
    MedianFilter<uint16_t> mixtureFilter(AdcMedianFilterSize);

    //display menu
    Menu menu(pDisplay);
    menu.registerItem(MenuId::Timer, "%Reset timer");
    menu.registerItem(MenuId::AircraftType, "$Aircraft type");
    menu.registerItem(MenuId::Heading, "$HDG %AP");

    //pushbutton objects
    PushButton menuLeft(HAT_SET_GPIO_Port, HAT_SET_Pin);
    PushButton menuRight(HAT_RST_GPIO_Port, HAT_RST_Pin);
    PushButton hatLeft(HAT_LEFT_GPIO_Port, HAT_LEFT_Pin);
    PushButton hatRight(HAT_RIGHT_GPIO_Port, HAT_RIGHT_Pin);
    PushButton hatMid(HAT_MID_GPIO_Port, HAT_MID_Pin);

    //info window object and data structure
    InfoWindow infoWindow(pDisplay);
    Timer pilotsTimer;
    pilotsTimer.reset();
    InfoMode mainInfoMode{InfoMode::Timer};
    InfoData infoData = {mainInfoMode, false};

    //Status object
    Status status(pDisplay);



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
        if((status.getAircraftType() == AircraftType::Engine) &&
           (reverseOn == false) &&      //reverser is off
           (HAL_GPIO_ReadPin(PB_RED_GPIO_Port, PB_RED_Pin) == GPIO_PinState::GPIO_PIN_RESET) &&       //reverse button is pressed
           (throttleFilter.getMedian() < ADC10Pct))     //throttle is < 10%
        {
            reverseOn = true;
            reverseOffArmed = false;
            infoData.mode = InfoMode::Reverser;
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
            infoData.mode = mainInfoMode;
        }

        //process USB reports
        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
        {
            //set game controller axes
            if(status.getAircraftType() == AircraftType::Glider)
            {
                //glider: use slide pot for air brakes
                gameController.data.Rx = scale<uint16_t, uint16_t>(0, Max12Bit, throttleFilter.getMedian(), 0, Max15Bit);
                gameController.data.slider = 0;
            }
            else
            {
                //anything but a glider - use slide pot for throttle
                gameController.data.slider = scale<uint16_t, uint16_t>(0, Max12Bit, throttleFilter.getMedian(), 0, Max15Bit);
                gameController.data.Rx = 0;
            }
            gameController.data.dial = scale<uint16_t, uint16_t>(0, Max12Bit, propellerFilter.getMedian(), 0, Max15Bit);
            gameController.data.Z = scale<uint16_t, int16_t>(0, Max12Bit, mixtureFilter.getMedian(), -Max15Bit, Max15Bit);

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
            if(menu.getItemId() == MenuId::Heading)
            {
                gameController.setButton(GameControllerButton::headingDec, HAL_GPIO_ReadPin(HAT_LEFT_GPIO_Port, HAT_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);   //dec heading
                gameController.setButton(GameControllerButton::headingInc, HAL_GPIO_ReadPin(HAT_RIGHT_GPIO_Port, HAT_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //inc heading
            }

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

        //handle menu change
        if(menuLeft.hasBeenPressed())
        {
            menu.decItem();
        }
        if(menuRight.hasBeenPressed())
        {
            menu.incItem();
        }

        //pilots timer reset
        if((menu.getItemId() == MenuId::Timer) &&
           (hatMid.hasBeenPressed()))
        {
            infoData.timerResetRequest = true;
        }

        //handle aircraft type change
        if(menu.getItemId() == MenuId::AircraftType)
        {
            if(hatLeft.hasBeenPressed())
            {
                status.changeAircraftType(-1);
            }

            if(hatRight.hasBeenPressed())
            {
                status.changeAircraftType(1);
            }
        }

        if(pDisplay->isOn())
        {

            //print new menu item
            if(menu.getItemIdx() != lastMenuItemIdx)
            {
                lastMenuItemIdx = menu.getItemIdx();
                menu.display();
            }

            //display info window
            infoWindow.handler(infoData);

            //handle status
            status.handler();
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
    if(hspi == pHspi2)
    {
        if(pDisplay != nullptr)
        {
            pDisplay->freeBus();
        }
    }
}
