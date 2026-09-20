#include "../include/DimOrbit/clock.h"
#include <format>

namespace DimOrbit {

// == TIME ==
uint64_t Time::time() const {
    return ms;
}
int Time::millisecond() const {
    return ms % 1000;
}
int Time::second() const {
    return (ms / 1'000) % 60;
}
int Time::minute() const {
    return (ms / 60'000) % 60;
}
int Time::hour() const {
    return (ms / 3'600'000) % 24;
}
std::string Time::getUTCTime() const {
    return std::string(std::format("{:02}", hour()) + ":" + std::format("{:02}", minute()) + ":" +
                       std::format("{:02}", second()) + "UTC");
}
std::string Time::getUTCTime_ms() const {
    return std::string(std::format("{:02}", hour()) + ":" + std::format("{:02}", minute()) + ":" +
                       std::format("{:02}", second()) + ":" + std::format("{:03}", millisecond()) +
                       "UTC");
}

Time& Time::advanceMs(int amount) {
    ms += amount;
    return *this;
}
Time& Time::advanceSec(int amount) {
    ms += amount * 1'000;
    return *this;
}
Time& Time::advanceMin(int amount) {
    ms += amount * 60'000;
    return *this;
}
Time& Time::advanceHr(int amount) {
    ms += amount * 3'600'000;
    return *this;
}
Time& Time::setTime(Time& newtime) {
    ms = newtime.ms;
    fractionAccum = newtime.fractionAccum;
    return *this;
}

Time Time::operator+(const Time& other) {
    return Time(ms + other.ms, fractionAccum);
}
Time Time::operator-(const Time& other) {
    return Time(ms - other.ms, fractionAccum);
}
Time Time::operator+(int mss) {
    return Time(ms + mss, fractionAccum);
}
Time Time::operator-(int mss) {
    return Time(ms - mss);
}
Time& Time::operator+=(int mss) {
    advanceMs(mss);
    return *this;
}
Time& Time::operator-=(int mss) {
    advanceMs(-mss);
    return *this;
}

int Time::tick(float delta) {
    int ticks = 0;
    fractionAccum += delta;
    while (fractionAccum >= 1.0f / 1000.0f) {
        ms++;
        ticks++;
        fractionAccum -= (1.0f / 1000.0f);
    }
    return ticks;
}

// == CLOCK ==
CelestialSystem& Clock::getSystem(const std::string& errormsg) {
    const std::string error =
        errormsg.empty() ? "Clock cannot access celestial system because it is empty" : errormsg;

    if (!system.has_value()) {
        throw std::runtime_error("DimOrbit: " + error);
    }
    return system.value().get();
}
bool Clock::hasSystem() {
    return system.has_value();
}
void Clock::setSystem(CelestialSystem& newsystem) {
    system = newsystem;
}
void Clock::setSpeedScale(float newscale) {
    speedScale = newscale;
}
void Clock::speedUp(float amount) {
    speedScale += amount;
}
void Clock::slowDown(float amount) {
    speedScale -= amount;
}
void Clock::start() {
    if (isRunning())
        return;
    speedScale = ssAccum;
}
void Clock::stop() {
    if (!isRunning())
        return;
    ssAccum = speedScale;
    speedScale = 0.0f;
}
bool Clock::isRunning() const {
    return speedScale != 0.0f;
}
uint64_t Clock::getTime() const {
    return time.time();
}

Clock::Clock(CelestialSystem& startSystem) {
    system = startSystem;
}

void Clock::tick(float delta) {
    int ticks = time.tick(delta * speedScale);
    for (int i = 0; i < ticks; i++) {
        // we tick by 1ms because ticks returns approximately the amount of milliseconds in delta
        getSystem().tick(0.001f);
    }
    tickBinds();
}
void Clock::bind(const ClockEvent& event) {
    binds.push_back(event);
}
void Clock::tickBinds() {
    for (auto& bind : binds) {
        if (bind.condition()) {
            bind.action();
        }
    }
}
void Clock::after(uint64_t ms, const std::function<void()>& action) {
    const uint64_t deadline = time.time() + ms;

    bind({
        .condition = [this, deadline]() { return time.time() >= deadline; },
        .action = [action]() { action(); },
    });
}
void Clock::doAt(uint64_t ms, const std::function<void()>& action) {
    bind({
        .condition = [this, ms]() { return time.time() >= ms; },
        .action = [action]() { action(); },
    });
}

} // namespace DimOrbit