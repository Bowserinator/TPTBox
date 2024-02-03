#ifndef FRAMETIMEAVG_H
#define FRAMETIMEAVG_H

#include <numeric>
#include "raylib.h"

constexpr std::size_t DELTA_SAMPLES_FOR_AVG = 10;
constexpr unsigned int TARGET_FPS = 300;

// For getting average time between frames for controls
// and other animations (so they can be framerate-independent-ish)
class FrameTime {
public:
    FrameTime(FrameTime& other) = delete;
    void operator=(const FrameTime&) = delete;
    ~FrameTime() = default;

    void update() {
        const float thisDelta = GetTime() - _lastTime;
        _deltaSamples[_deltaSampleIdx] = thisDelta;
        _deltaSampleIdx = (_deltaSampleIdx + 1) % DELTA_SAMPLES_FOR_AVG;
        deltaAvg = std::accumulate(&_deltaSamples[0], &_deltaSamples[DELTA_SAMPLES_FOR_AVG], 0.0f) / DELTA_SAMPLES_FOR_AVG;
        _lastTime = GetTime();
    }

    float getDelta() const { return deltaAvg; }

    static FrameTime * ref() {
        if (single == nullptr) [[unlikely]]
            single = new FrameTime;
        return single;
    };
private:
    float _deltaSamples[DELTA_SAMPLES_FOR_AVG];
    int _deltaSampleIdx;
    double _lastTime;

    float deltaAvg;
    inline static FrameTime * single = nullptr;

    FrameTime(): _deltaSampleIdx(0), _lastTime(0) {
        std::fill(&_deltaSamples[0], &_deltaSamples[DELTA_SAMPLES_FOR_AVG], 0.0f);
    }
};

#endif