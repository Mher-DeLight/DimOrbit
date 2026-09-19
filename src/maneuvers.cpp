#include "../include/DimOrbit/maneuvers.h"
#include "../include/DimOrbit/spacecraft.h"

namespace DimOrbit::maneuver {
bool TimeManeuver::shouldApply(BasicSpacecraft& craft, Clock& clock) const {
    return applicationTime.time() >= clock.time.time();
}
void TimeManeuver::apply(BasicSpacecraft& craft, Clock& clock) {
    craft.engine.applyMaxThrust(deltaV.velocity);
}
} // namespace DimOrbit::maneuver