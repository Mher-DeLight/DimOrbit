#pragma once
#include "clock.h"
#include "common.h"

namespace DimOrbit::manuever {

struct DeltaV {
    Vec3 velocity = Vec3::ZERO;
    DeltaV(const Vec3& vel) : velocity(vel) {}
};
struct Manuever {
    virtual bool shouldApply(BasicSpacecraft& craft, Clock& clock) const = 0;
    virtual void apply(BasicSpacecraft& craft, Clock& clock) = 0;

    virtual ~Manuever() = default;
};
struct TimeManuever : public Manuever {
    // yes, i'm using inheritance. it's none of your business.
    Time applicationTime;
    DeltaV deltaV;

    bool shouldApply(BasicSpacecraft& craft, Clock& clock) const override;
    void apply(BasicSpacecraft& craft, Clock& clock) override;

    TimeManuever(const Time& applicationTime_, const DeltaV& deltaV_)
        : applicationTime(applicationTime_), deltaV(deltaV_) {}
};

}; // namespace DimOrbit::manuever