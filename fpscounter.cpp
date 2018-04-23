#include "fpscounter.h"

FPSCounter::FPSCounter()
{
    t2 = std::chrono::steady_clock::now();
    t1 = std::chrono::steady_clock::now();
}

double FPSCounter::frame() {
    t1 = t2;
    t2 = std::chrono::steady_clock::now();

    double msecs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 -t1).count();
    return 1.0 / (msecs / 1000.0);
}

