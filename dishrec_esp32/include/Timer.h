#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class Timer
{
protected:
    bool _started;
    std::chrono::duration<long long, std::nano> _duration, _mean;
    std::chrono::high_resolution_clock::time_point _startPoint;
    void _start();
    void _stop();
    void _avg();
public:
    Timer();
    ~Timer();
    void start();
    void stop();
    void print(const char* action = nullptr);
    void tick();
    void reset();
};

#endif