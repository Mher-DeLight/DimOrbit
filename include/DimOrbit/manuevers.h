#pragma once
#include "clock.h"
#include "common.h"

namespace DimOrbit::manuever {

struct DeltaV {
    Vec3 velocity = Vec3::ZERO;
    DeltaV(const Vec3& vel) : velocity(vel) {}
};
struct Manuever {
    virtual bool shouldApply(BasicSpacecraft& craft, CelestialSystem& system,
                             Clock& clock) const = 0;
    virtual void apply(BasicSpacecraft& craft, CelestialSystem& system, Clock& clock) = 0;
    virtual void tick(BasicSpacecraft& craft, CelestialSystem& system, Clock& clock) {
        if (shouldApply(craft, system, clock)) {
            apply(craft, system, clock);
        }
    }

    virtual ~Manuever() = default;
};
struct TimeManuever : public Manuever {
    // yes, i'm using inheritance. it's none of your business.
    Time applicationTime;
    DeltaV deltaV;

    bool shouldApply(BasicSpacecraft& craft, CelestialSystem& system, Clock& clock) const override {
        return applicationTime.time() >= clock.time.time()
    }
    void apply(BasicSpacecraft& craft, CelestialSystem& system, Clock& clock) override {
        craft.engine.applyMaxThrust(deltaV.velocity);
    }

    TimeManuever(const Time& applicationTime_, const DeltaV& deltaV_)
        : applicationTime(applicationTime_), deltaV(deltaV_) {}
};

}; // namespace DimOrbit::manuever