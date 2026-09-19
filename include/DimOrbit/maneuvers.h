#pragma once
#include "clock.h"
#include "common.h"

namespace DimOrbit::maneuver {

struct DeltaV {
    Vec3 velocity = Vec3::ZERO;
    DeltaV(const Vec3& vel) : velocity(vel) {}
};
struct Maneuver {
    virtual bool shouldApply(BasicSpacecraft& craft, Clock& clock) const = 0;
    virtual void apply(BasicSpacecraft& craft, Clock& clock) = 0;

    virtual ~Maneuver() = default;
};
struct ArbitraryTimeManeuver : public Maneuver {
    Time applicationTime;
    std::function<void()> action;
    bool applied = false;

    bool shouldApply(BasicSpacecraft& craft, Clock& clock) const override;
    void apply(BasicSpacecraft& craft, Clock& clock) override;

    ArbitraryTimeManeuver(const Time& applicationTime_, const std::function<void()> action_)
        : applicationTime(applicationTime_), action(action_) {}
};
struct TimeManeuver : public Maneuver {
    // yes, i'm using inheritance. it's none of your business.
    Time applicationTime;
    DeltaV deltaV;
    bool applied = false;

    bool shouldApply(BasicSpacecraft& craft, Clock& clock) const override;
    void apply(BasicSpacecraft& craft, Clock& clock) override;

    TimeManeuver(const Time& applicationTime_, const DeltaV& deltaV_)
        : applicationTime(applicationTime_), deltaV(deltaV_) {}
};

}; // namespace DimOrbit::maneuver