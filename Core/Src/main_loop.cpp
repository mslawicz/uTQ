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
static float throttleFiltered = 0;
static float propellerFiltered = 0;
static float mixtureFiltered = 0;
static float miniJoyXFiltered = 0;
static float miniJoyYFiltered = 0;

void mainLoop()
{
    constexpr uint32_t HeartbeatPeriod = 500000U;
    constexpr uint32_t AdcPeriod = GameController::ReportInterval / 10;
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

    //display menu
    Menu menu(pDisplay);
    menu.registerItem(MenuId::Timer, "%Reset timer");
    menu.registerItem(MenuId::AircraftType, "$Aircraft type");
    menu.registerItem(MenuId::Heading, "$HDG %AP");
    menu.registerItem(MenuId::Altitude, "$ALT #VS %AP");

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

            //filter ADC data
            throttleFiltered += AlphaEMA * (1.0f - static_cast<float>(adcConvBuffer[throttle]) / Max12BitF - throttleFiltered);
            propellerFiltered += AlphaEMA * (static_cast<float>(adcConvBuffer[propeller]) / Max12BitF - propellerFiltered);
            mixtureFiltered += AlphaEMA * (static_cast<float>(adcConvBuffer[mixture]) / Max12BitF - mixtureFiltered);
            miniJoyXFiltered += AlphaEMA * (static_cast<float>(adcConvBuffer[miniJoyX]) / Max12BitF - miniJoyXFiltered);
            miniJoyYFiltered += AlphaEMA * (static_cast<float>(adcConvBuffer[miniJoyY]) / Max12BitF - miniJoyYFiltered);

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
           (throttleFiltered < ADC10Pct))     //throttle is < 10%
        {
            reverseOn = true;
            reverseOffArmed = false;
            infoData.mode = InfoMode::Reverser;
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
            infoData.mode = mainInfoMode;
        }

        //process linear brakes
        float brakeLeft = ((miniJoyXFiltered < 0.5f) ? 2.0f * (0.5f - miniJoyXFiltered) : 0) +		//mini joystick moved forward
        						  ((miniJoyYFiltered > 0.5f) ? 2.0f * (miniJoyYFiltered - 0.5f) : 0);			//mini joystick moved to left
        float brakeRight = ((miniJoyXFiltered < 0.5f) ? 2.0f * (0.5f - miniJoyXFiltered) : 0) +		//mini joystick moved forward
        						  ((miniJoyYFiltered < 0.5f) ? 2.0f * (0.5f - miniJoyYFiltered) : 0);			//mini joystick moved to right

        //process USB reports
        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
        {
            //set game controller axes
            if(status.getAircraftType() == AircraftType::Glider)
            {
                //glider: use slide pot for air brakes
                gameController.data.Rz = scale<float, int16_t>(0, 1.0f, throttleFiltered, -Max15Bit, Max15Bit);
                gameController.data.slider = 0;
            }
            else
            {
                //anything but a glider - use slide pot for throttle
                gameController.data.slider = scale<float, uint16_t>(0, 1.0f, throttleFiltered, 0, Max15Bit);
                gameController.data.Rz = 0;
            }
            gameController.data.dial = scale<float, uint16_t>(0, 1.0f, propellerFiltered, 0, Max15Bit);
            gameController.data.Z = scale<float, int16_t>(0, 1.0f, mixtureFiltered, -Max15Bit, Max15Bit);
            gameController.data.Rx = scale<float, uint16_t>(0.05f, 1.0f, brakeLeft, 0, Max15Bit);
            gameController.data.Ry = scale<float, uint16_t>(0.05f, 1.0f, brakeRight, 0, Max15Bit);

            //set game controller buttons
            gameController.data.buttons = 0;
            gameController.setButton(GameControllerButton::reverser, reverseOn);
            gameController.setButton(GameControllerButton::flapsUp, HAL_GPIO_ReadPin(FLAPS_UP_GPIO_Port, FLAPS_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::flapsDown, HAL_GPIO_ReadPin(FLAPS_DOWN_GPIO_Port, FLAPS_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearUp, HAL_GPIO_ReadPin(GEAR_UP_GPIO_Port, GEAR_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::gearDown, HAL_GPIO_ReadPin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::blueButton, HAL_GPIO_ReadPin(PB_BLUE_GPIO_Port, PB_BLUE_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::leftToggle, HAL_GPIO_ReadPin(TOGGLE_LEFT_GPIO_Port, TOGGLE_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            gameController.setButton(GameControllerButton::rightToggle, HAL_GPIO_ReadPin(TOGGLE_RIGHT_GPIO_Port, TOGGLE_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            if(menu.getItemId() == MenuId::Heading)
            {
                gameController.setButton(GameControllerButton::headingDec, HAL_GPIO_ReadPin(HAT_LEFT_GPIO_Port, HAT_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);   //dec heading
                gameController.setButton(GameControllerButton::headingInc, HAL_GPIO_ReadPin(HAT_RIGHT_GPIO_Port, HAT_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //inc heading
                gameController.setButton(GameControllerButton::apHeading, HAL_GPIO_ReadPin(HAT_MID_GPIO_Port, HAT_MID_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //AP heading mode
            }
            if(menu.getItemId() == MenuId::Altitude)
            {
                gameController.setButton(GameControllerButton::altitudeDec, HAL_GPIO_ReadPin(HAT_LEFT_GPIO_Port, HAT_LEFT_Pin) == GPIO_PinState::GPIO_PIN_RESET);   //dec altitude
                gameController.setButton(GameControllerButton::altitudeInc, HAL_GPIO_ReadPin(HAT_RIGHT_GPIO_Port, HAT_RIGHT_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //inc altitude
                gameController.setButton(GameControllerButton::vsDec, HAL_GPIO_ReadPin(HAT_UP_GPIO_Port, HAT_UP_Pin) == GPIO_PinState::GPIO_PIN_RESET);   //dec vs
                gameController.setButton(GameControllerButton::vsInc, HAL_GPIO_ReadPin(HAT_DOWN_GPIO_Port, HAT_DOWN_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //inc vs
                gameController.setButton(GameControllerButton::apVs, HAL_GPIO_ReadPin(HAT_MID_GPIO_Port, HAT_MID_Pin) == GPIO_PinState::GPIO_PIN_RESET);  //AP vs mode
            }
            if(status.getAircraftType() == AircraftType::Glider)
            {
                gameController.setButton(GameControllerButton::towRelease, HAL_GPIO_ReadPin(PB_GREEN_GPIO_Port, PB_GREEN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            }
            else
            {
                //not a glider
                gameController.setButton(GameControllerButton::greenButton, HAL_GPIO_ReadPin(PB_GREEN_GPIO_Port, PB_GREEN_Pin) == GPIO_PinState::GPIO_PIN_RESET);
            }
            gameController.setButton(GameControllerButton::miniJoyPB, HAL_GPIO_ReadPin(MINI_JOY_PB_GPIO_Port, MINI_JOY_PB_Pin) == GPIO_PinState::GPIO_PIN_RESET);

            // unused data members
            gameController.data.X = 0;
            gameController.data.Y = 0;
            gameController.data.hat = 0;

            gameController.sendReport();
            gameCtrlTimer.reset();
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
