#include "Timer.h"

Timer::Timer() :
_started(false),
_mean(0)
{
    _start();
}

Timer::~Timer()
{
}

void Timer::_start()
{
    this->_startPoint = std::chrono::high_resolution_clock::now();
}

void Timer::_stop()
{
    this->_duration = (
            std::chrono::high_resolution_clock::now()
            - this->_startPoint
        );
}

void Timer::_avg()
{
    this->_mean += this->_duration;
    this->_mean /= 2;
}

void Timer::start()
{
    _start();
}

void Timer::stop()
{
    _stop();
    _avg();
}

void Timer::tick()
{
    _stop();
    if (!this->_started)
    {
        this->_started = true;
        this->_mean = this->_duration;
        _start();
        return;
    }
    _avg();
    _start();
}

void Timer::print(const char* action)
{
    if (action != nullptr) std::cout << action << " ";
    else std::cout << "Timer ";
    std::cout << "averaged " << this->_mean.count() << " ns" << std::endl;
}

void Timer::reset()
{
    this->_started = false;
    this->_mean = std::chrono::duration<long long, std::nano>(0);
}