#include "../include/DimOrbit/clock.h"

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

void Time::tick(float delta) {
    fractionAccum += delta;
    while (fractionAccum * 1000 > 1.0f) {
        ms++;
        fractionAccum -= (1 / 1000);
    }
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

Clock::Clock(CelestialSystem& startSystem) {
    system = startSystem;
}

void Clock::tick(float delta) {
    getSystem().tick(delta);
}

} // namespace DimOrbit