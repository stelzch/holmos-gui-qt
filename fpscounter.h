#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <chrono>

class FPSCounter
{
public:
    FPSCounter();
    double frame();

private:
    std::chrono::time_point<std::chrono::steady_clock> t1, t2;
};

#endif // FPSCOUNTER_H
