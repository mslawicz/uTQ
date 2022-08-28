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
//XXX #include "pc_link.h"
//XXX #include "convert.h"
//XXX #include "constant.h"
#include "logger.h"
//XXX #include "monitor.h"

//XXX ADC_HandleTypeDef* pHadc;    //pointer to ADC object
//XXX uint16_t adcConvBuffer[MAX_ADC_CH]; //buffer for ADC conversion results

#ifdef MONITOR
#endif

void mainLoop()
{
    constexpr uint32_t HeartbeatPeriod = 500000;
    Timer statusLedTimer;
    Timer gameCtrlTimer;

    LOG_ALWAYS("uTQ v1.0");

    //assign system LEDs
    GPIO_TypeDef* heartbeatLedPort = LD2_GPIO_Port; //green LED
    uint16_t heartbeatLedPin = LD2_Pin;

    //XXX GameController gameController;  //USB link-to-PC object (class custom HID - joystick)

    Timer::start(pTimerHtim);

    /* main forever loop */
    while(true)
    {
#ifdef MONITOR
#endif
        /* request next conversions of analog channels */
        //XXX HAL_ADC_Start_DMA(pHadc, (uint32_t*)adcConvBuffer, pHadc->Init.NbrOfConversion);

        if(statusLedTimer.hasElapsed(HeartbeatPeriod))
        {
            HAL_GPIO_TogglePin(heartbeatLedPort, heartbeatLedPin);
            statusLedTimer.reset();
        }

//        if(gameCtrlTimer.hasElapsed(GameController::ReportInterval))
//        {
//            gameController.sendReport();
//            gameCtrlTimer.reset();
//        }

    }
}

