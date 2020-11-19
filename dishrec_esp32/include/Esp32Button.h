#ifndef ESP32BUTTON_H
#define ESP32BUTTON_H

#include <chrono>

#ifdef ESP32
#include "driver/gpio.h"
#else
typedef int gpio_num_t;
#endif

/* Simple ESP32 input buttons

Template parameters are pin number and
active state (defaults to active when high)

Explicit template instantiations
extend up to GPIO46 to include ESP32-S2
pin numbers */

namespace Esp32Button
{

class SimpleButton
{
protected:
    gpio_num_t _pin;
    bool _activeState, _state;

public:
    SimpleButton();
    SimpleButton(int pinNum, bool activeHigh = false);
    ~SimpleButton();

    void set_pin(int pinNum);
    void set_active_state(bool activeHigh);

    virtual bool read();
    virtual void set(bool state);
    virtual bool get() const;
    virtual void toggle();

    operator bool() const;
    bool operator!() const;
};

/* A button that must be held momentarily to change state */

class MomentaryButton : public SimpleButton
{
protected:
    std::chrono::milliseconds _holdDurationRequired;
    
    virtual bool _hold_duration_met();

public:
    MomentaryButton();
    MomentaryButton(int pinNum, bool activeHigh = false);
    ~MomentaryButton();
    
    virtual void set_hold_duration_ms(int ms);
    virtual bool read_hold();
};

/* Momentary button with an external override trigger */

class DualActionButton : public MomentaryButton
{
protected:
    bool* _trigger;
    
public:
    DualActionButton();
    DualActionButton(int pinNum, bool activeHigh = false);
    DualActionButton(bool* target);
    ~DualActionButton();
    
    virtual void set_trigger(bool* target);
    virtual bool is_triggered();
    bool read() override;
    virtual bool get();
    bool read_hold() override;
};

};

#endif
