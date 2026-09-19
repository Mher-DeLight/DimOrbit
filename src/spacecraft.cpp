#include "../include/DimOrbit/spacecraft.h"

namespace DimOrbit {

// == BASIC SPACECRAFT ==
void BasicSpacecraft::tick(float delta) {
    if (engine.maxThrust.x == 0 && engine.maxThrust.y == 0 && engine.maxThrust.z == 0)
        return; // if there is no thrust, movement will be handled by gravitate_Newtonian
    if (fuelTank.isEmpty())
        return;

    physics->core.applyForce(engine.thrust(), delta);
    fuelTank.consumeFuel(fuelElapseRate * delta);
}

maneuver::TimeManeuver BasicSpacecraft::createTimeManeuver(const Time& time,
                                                           const maneuver::DeltaV& deltaV,
                                                           Clock& clock) {
    auto maneuver = std::make_shared<maneuver::TimeManeuver>(time, deltaV);

    clock.bind({
        .condition = [this, maneuver, &clock]() { return maneuver->shouldApply(*this, clock); },
        .action = [this, maneuver, &clock]() { maneuver->apply(*this, clock); },
    });

    return *maneuver;
}

} // namespace DimOrbit