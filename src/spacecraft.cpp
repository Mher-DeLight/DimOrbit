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

manuever::TimeManuever BasicSpacecraft::createTimeManuever(const Time& time,
                                                           const manuever::DeltaV& deltaV,
                                                           Clock& clock) {
    auto manuever = std::make_shared<manuever::TimeManuever>(time, deltaV);

    clock.bind({
        .condition = [this, manuever, &clock]() { return manuever->shouldApply(*this, clock); },
        .action = [this, manuever, &clock]() { manuever->apply(*this, clock); },
    });

    return *manuever;
}

} // namespace DimOrbit