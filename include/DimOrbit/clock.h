#pragma once
#include "common.h"
#include <optional>

namespace DimOrbit {

struct Clock {
    std::optional<std::reference_wrapper<CelestialSystem>> system;
    float speedScale = 1.0; // single precision is enough i think

    // configuration
    CelestialSystem& getSystem(const std::string& errormsg = "");
    bool hasSystem();
    void setSystem(CelestialSystem& newsystem);

    // actions
    void tick(float delta);

    // constructors
    Clock() = default;
    Clock(CelestialSystem& startSystem);
};

}; // namespace DimOrbit