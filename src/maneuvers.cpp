#include "../include/DimOrbit/maneuvers.h"
#include "../include/DimOrbit/spacecraft.h"

namespace DimOrbit::maneuver {
bool TimeManeuver::shouldApply(BasicSpacecraft& craft, Clock& clock) const {
    return !applied && clock.time.time() >= applicationTime.time();
}
void TimeManeuver::apply(BasicSpacecraft& craft, Clock& clock) {
    craft.engine.applyMaxThrust(deltaV.velocity);
    applied = true;
}
} // namespace DimOrbit::maneuver