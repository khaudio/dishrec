#include "Esp32Button.h"

using namespace Esp32Button;

SimpleButton::SimpleButton() :
_activeState(false),
_state(false)
{
}

SimpleButton::SimpleButton(int pinNum, bool activeHigh) :
SimpleButton()
{
    this->_pin = static_cast<gpio_num_t>(pinNum);
    this->_activeState = activeHigh;
    
    #ifdef ESP32
    gpio_config_t inputConf;
    inputConf.pin_bit_mask = (1ULL << this->_pin);
    inputConf.mode = GPIO_MODE_INPUT;
    inputConf.intr_type = static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE);
    if (this->_activeState)
    {
        inputConf.pull_up_en = GPIO_PULLUP_DISABLE;
        inputConf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else
    {
        inputConf.pull_up_en = GPIO_PULLUP_ENABLE;
        inputConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    gpio_config(&inputConf);
    #endif

    read();
}

SimpleButton::~SimpleButton()
{
}

void SimpleButton::set_pin(int pinNum)
{
    this->_pin = static_cast<gpio_num_t>(pinNum);
}

void SimpleButton::set_active_state(bool activeHigh)
{
    this->_activeState = activeHigh;
}

inline bool SimpleButton::read()
{
    #ifdef ESP32
    this->_state = this->_activeState == gpio_get_level(this->_pin);
    #endif
    return this->_state;
}

inline void SimpleButton::set(bool state)
{
    this->_state = state;
}

inline bool SimpleButton::get() const
{
    return this->_state;
}

inline void SimpleButton::toggle()
{
    /* Manually toggle state,
    rather than reading input pin */
    this->_state = !this->_state;
}

SimpleButton::operator bool() const
{
    return this->_state;
}

bool SimpleButton::operator!() const
{
    return !(this->_state);
}

MomentaryButton::MomentaryButton() :
SimpleButton(),
_holdDurationRequired(1)
{
}

MomentaryButton::MomentaryButton(int pinNum, bool activeHigh) :
SimpleButton(pinNum, activeHigh)
{
}

MomentaryButton::~MomentaryButton()
{
}

void MomentaryButton::set_hold_duration_ms(int ms)
{
    this->_holdDurationRequired = std::chrono::milliseconds(ms); 
}

bool MomentaryButton::_hold_duration_met()
{
    /* Measure hold duration
    Returns true only when duration is met
    Continuing to hold will return false */
    static bool reading(false);
    static std::chrono::high_resolution_clock::time_point start;
    if (!reading)
    {
        start = std::chrono::high_resolution_clock::now();
        reading = true;
    }
    else
    {
        if (
                (std::chrono::high_resolution_clock::now() - start)
                >= this->_holdDurationRequired
            )
        {
            this->toggle();
            reading = false;
            return true;
        }
    }
    return false;
}

bool MomentaryButton::read_hold()
{
    /* Returns true when button has been
    held for specified duration */
    static bool level(false);
    #ifdef ESP32
    level = gpio_get_level(this->_pin);
    #endif
    if (level != this->_activeState)
    {
        this->_state = false;
    }
    else if (this->_state != level)
    {
        return this->_hold_duration_met();
    }
    return false;
}

DualActionButton::DualActionButton() :
MomentaryButton()
{
    this->_trigger = nullptr;
}

DualActionButton::DualActionButton(int pinNum, bool activeHigh) :
MomentaryButton(pinNum, activeHigh)
{
}

DualActionButton::DualActionButton(bool* target) :
MomentaryButton(),
_trigger(target)
{
}

DualActionButton::~DualActionButton()
{
    this->_trigger = nullptr;
}

void DualActionButton::set_trigger(bool* target)
{
    this->_trigger = target;
}

inline bool DualActionButton::is_triggered()
{
    return ((this->_trigger != nullptr) && *this->_trigger);
}

inline bool DualActionButton::read()
{
    SimpleButton::read();
    return get();
}

inline bool DualActionButton::get()
{
    this->_state = (is_triggered() ? true : this->_state);
    return this->_state;
}

bool DualActionButton::read_hold() 
{
    /* Returns true when button has been
    either held or triggered externally
    for specified duration */
    static bool level(false);
    #ifdef ESP32
    level = gpio_get_level(this->_pin);
    #endif
    if (level != this->_activeState)
    {
        this->_state = false;
    }
    else if ((this->_state != level) || is_triggered())
    {
        return MomentaryButton::_hold_duration_met();
    }
    return false;
}
