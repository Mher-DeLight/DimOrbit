#pragma once
#include "common.h"
#include <vector>

namespace DimOrbit::gravity {
// == CONSTANTS ==
extern const double G;

// == VARIABLES ==
extern std::vector<DimOrbit::CelestialBody*> bodyList;

// == FUNCTIONS ==
void registerBody(DimOrbit::CelestialBody* body);
void tick(float delta);

} // namespace DimOrbit::gravity