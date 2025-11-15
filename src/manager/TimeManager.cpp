#pragma once

#include <chrono>

class TimeManager {
private:
    using clock = std::chrono::steady_clock;
    clock::time_point previousTime;
    bool initialized = false;

public:
    double tick() {
        const auto now = clock::now();
        if (!initialized) {
            previousTime = now;
            initialized = true;
            return 0.0;
        }

        const std::chrono::duration<double> delta = now - previousTime;
        previousTime = now;
        return delta.count();
    }

    void reset() {
        initialized = false;
    }
};
