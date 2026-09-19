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

bool ArbitraryTimeManeuver::shouldApply(BasicSpacecraft& craft, Clock& clock) const {
    return !applied && clock.time.time() >= applicationTime.time();
}
void ArbitraryTimeManeuver::apply(BasicSpacecraft& craft, Clock& clock) {
    action();
    applied = true;
}

} // namespace DimOrbit::maneuver