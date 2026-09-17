#include "../include/DimOrbit/clock.h"

namespace DimOrbit {

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