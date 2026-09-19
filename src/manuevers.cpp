#include "../include/DimOrbit/manuevers.h"
#include "../include/DimOrbit/spacecraft.h"

namespace DimOrbit::manuever {
bool TimeManuever::shouldApply(BasicSpacecraft& craft, Clock& clock) const {
    return applicationTime.time() >= clock.time.time();
}
void TimeManuever::apply(BasicSpacecraft& craft, Clock& clock) {
    craft.engine.applyMaxThrust(deltaV.velocity);
}
} // namespace DimOrbit::manuever