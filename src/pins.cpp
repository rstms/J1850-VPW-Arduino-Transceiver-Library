/*************************************************************************
**  AVR J1850VPW VPW Interface
**  by Stepan Matafonov
**
**  Released under Microsoft Public License
**
**  contact: matafonoff@gmail.com
**  homepage: xelb.ru
**************************************************************************/

#include <Arduino.h>

#include "pins.h"

#define MAX_INTERRUPTS 3

volatile uint8_t isr_count = 0;

portMUX_TYPE sync0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE sync1 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE sync2 = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t isr_pin[MAX_INTERRUPTS] = {0};
volatile pCallbackFunction isr_callback[MAX_INTERRUPTS] = {0};
void *isr_data[MAX_INTERRUPTS] = {0};
bool isr_invert[MAX_INTERRUPTS] = {0};

void IRAM_ATTR ISR0(void) 
{
    portENTER_CRITICAL(&sync0);
    uint8_t value = digitalRead(isr_pin[0]);
    if (isr_invert[0]) value = !value;
    isr_callback[0](value, isr_data[0]);
    portEXIT_CRITICAL(&sync0);
}

void IRAM_ATTR ISR1(void) 
{
    portENTER_CRITICAL(&sync1);
    uint8_t value = digitalRead(isr_pin[1]);
    if (isr_invert[1]) value = !value;
    isr_callback[1](value, isr_data[1]);
    portEXIT_CRITICAL(&sync1);
}

void IRAM_ATTR ISR2(void) 
{
    portENTER_CRITICAL(&sync2);
    uint8_t value = digitalRead(isr_pin[2]);
    if (isr_invert[2]) value = !value;
    isr_callback[2](value, isr_data[2]);
    portEXIT_CRITICAL(&sync2);
}

bool Pin::isEmpty() const
{
    return _index == -1;
}

void Pin::write(uint8_t val)
{
    if (_invert) val = !val;
    digitalWrite(isr_pin[_index], val ? HIGH : LOW);
}

uint8_t Pin::read()
{
    uint8_t value = digitalRead(isr_pin[_index]);
    if (_invert) value = !value;
    return value;
}

bool Pin::_attach()
{
    switch(_index) {
        case 0:
            attachInterrupt(isr_pin[_index], ISR0, _trigger); 
            break;
        case 1:
            attachInterrupt(isr_pin[_index], ISR1, _trigger); 
            break;
        case 2:
            attachInterrupt(isr_pin[_index], ISR2, _trigger); 
            break;
        default:
            return false;
    }
    _attached = true;
    return true;
}

bool Pin::attach(PIN_CHANGE changeType, pCallbackFunction onPinChanged, void* pData)
{
    _trigger = (uint8_t)changeType;
    isr_callback[_index] = onPinChanged;
    isr_data[_index] = pData;
    return _attach();
}

void Pin::_detach() 
{
    if (_attached) {
        detachInterrupt(isr_pin[_index]);
        _attached = false;
    }
}

void Pin::detach()
{
    _detach();
}


void Pin::resumeInterrupts()
{
    _attach();
}

void Pin::pauseInterrupts()
{
    _detach();
}

Pin ::~Pin()
{
    if (_index != -1) {
        _detach();
        _index = -1;
    }
}

Pin::Pin()
{
    _index = -1;
    _attached = false;
}

Pin::Pin(uint8_t pin, PIN_MODES mode, bool invert)
{
    _index = -1;
    _attached = false;
    _invert = invert;
    if (isr_count < MAX_INTERRUPTS) {
        _index = isr_count++;
        isr_pin[_index] = pin;
        isr_invert[_index] = _invert;
        _mode = (uint8_t)mode;
        pinMode(pin, _mode);
    }
}
