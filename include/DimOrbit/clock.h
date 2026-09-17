#pragma once
#include "common.h"
#include <cstdint>
#include <optional>

namespace DimOrbit {

struct Time {
    int tick(float delta);

    int hour() const;
    int minute() const;
    int second() const;
    int millisecond() const;
    uint64_t time() const;

    Time& advanceMs(int amount);
    Time& advanceSec(int amount);
    Time& advanceMin(int amount);
    Time& advanceHr(int amount);
    Time& setTime(Time& newtime);

    Time operator+(const Time& other);
    Time operator-(const Time& other);
    Time operator+(int mss);
    Time operator-(int mss);
    Time& operator+=(int mss);
    Time& operator-=(int mss);

    Time() = default;
    Time(uint64_t ms_, double fractionAccum_ = 0.0) {
        ms = ms_;
        fractionAccum = fractionAccum_;
    }

private:
    uint64_t ms = 0;
    float fractionAccum = 0.0;
};

struct Clock {
    std::optional<std::reference_wrapper<CelestialSystem>> system;
    float speedScale = 1.0; // single precision is enough i think
    Time time;

    // configuration
    CelestialSystem& getSystem(const std::string& errormsg = "");
    bool hasSystem();
    void setSystem(CelestialSystem& newsystem);
    void setSpeedScale(float newscale);
    void speedUp(float amount);
    void slowDown(float amount);
    bool isRunning() const;
    void stop();
    void start();
    uint64_t getTime() const;

    // actions
    void tick(float delta);

    // constructors
    Clock() = default;
    Clock(CelestialSystem& startSystem);

private:
    float ssAccum = 1.0f;
};

}; // namespace DimOrbit