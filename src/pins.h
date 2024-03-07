/*************************************************************************
**  AVR J1850VPW VPW Interface
**  by Stepan Matafonov
**
**  Released under Microsoft Public License
**
**  contact: matafonoff@gmail.com
**  homepage: xelb.ru
**************************************************************************/
#pragma once
#include <Arduino.h>


enum PIN_CHANGE {
    PIN_CHANGE_BOTH = CHANGE,
    PIN_CHANGE_RISE = RISING,
    PIN_CHANGE_FALL = FALLING
};

enum PIN_MODES {
    PIN_MODE_INPUT = INPUT,
    PIN_MODE_INPUT_PULLUP = INPUT_PULLUP,
    PIN_MODE_OUTPUT = OUTPUT,
};

typedef void (*pCallbackFunction)(int state, void* pData);

class Pin {
private:
    int _index;
    uint8_t _mode;
    uint8_t _trigger;
    bool _attached;
    bool _attach();
    void _detach();
    bool _invert;

public:
    Pin();
    Pin(uint8_t pin, PIN_MODES mode, bool invert);
    ~Pin();
public:
    void write(uint8_t val);
    uint8_t read();

    bool isEmpty() const;

    bool attach(PIN_CHANGE changeType, pCallbackFunction onPinChaged, void* pData);
    void detach();

    void resumeInterrupts();
    void pauseInterrupts();

};

