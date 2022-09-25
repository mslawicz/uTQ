/*
 * rotary_encoder.h
 *
 *  Created on: 24 wrz 2022
 *      Author: marci
 */

#ifndef INC_ROTARY_ENCODER_H_
#define INC_ROTARY_ENCODER_H_

#include "stm32f4xx_hal.h"
#include "timer.h"

class RotaryEncoder
{
public:
    RotaryEncoder
    (
            GPIO_TypeDef* clkPort, uint16_t clkPin,
            GPIO_TypeDef* dtPort, uint16_t dtPin
    );
    void handler();
    int16_t getPulseCounter() const { return _pulseCounter; }
private:
    enum class RotEncState : uint8_t
    {
        Stable,
        Changed
    };
    GPIO_TypeDef* _clkPort;
    uint16_t _clkPin;
    GPIO_TypeDef* _dtPort;
    uint16_t _dtPin;
    RotEncState _state{RotEncState::Stable};
    GPIO_PinState _lastClkValue{GPIO_PinState::GPIO_PIN_SET};   //last value of clk pin
    GPIO_PinState _newClkValue{GPIO_PinState::GPIO_PIN_SET};    //clk value recorded on clk change after stable period
    GPIO_PinState _newDtValue{GPIO_PinState::GPIO_PIN_SET};    //data value recorded on clk change after stable period
    Timer _stabilityTimer;
    static constexpr uint32_t StabilityTime{2000};      //requested clock stability time
    int16_t _pulseCounter{0};   //counts rotation pulses
    static constexpr uint32_t ShortPeriod{30000};
    static constexpr uint32_t MediumPeriod{60000};
    Timer _lastChangeTimer;
};


#endif /* INC_ROTARY_ENCODER_H_ */
