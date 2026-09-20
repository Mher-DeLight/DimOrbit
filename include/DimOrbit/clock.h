#pragma once
#include "common.h"
#include <cstdint>
#include <functional>
#include <optional>

namespace DimOrbit {

struct Time {
    int tick(float delta);

    int hour() const;
    int minute() const;
    int second() const;
    int millisecond() const;
    uint64_t time() const;
    std::string getUTCTime() const;
    std::string getUTCTime_ms() const;

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
    double fractionAccum = 0.0;
};

struct ClockEvent {
    std::function<bool()> condition;
    std::function<void()> action;
};

struct Clock {
    std::optional<std::reference_wrapper<CelestialSystem>> system;
    float speedScale = 1.0; // single precision is enough i think
    Time time;
    std::vector<ClockEvent> binds;
    std::vector<ClockEvent> realBinds;

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
    uint64_t realTime() const;
    const Time& realTimer() const;

    // actions
    void tick(float delta);
    void bind(const ClockEvent& event);
    void tickBinds();
    void tickRealBinds();
    void after(uint64_t ms, const std::function<void()>& action);
    void doAt(uint64_t ms, const std::function<void()>& action);
    void doAtRealTime(uint64_t ms, const std::function<void()>& action);

    // constructors
    Clock() = default;
    Clock(CelestialSystem& startSystem);

private:
    float ssAccum = 1.0f;
    Time _realTime;
};

}; // namespace DimOrbit