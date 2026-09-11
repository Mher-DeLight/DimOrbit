#pragma once
#include "DimOrbit.h"
#include <vector>

namespace DimOrbit::gravity {
// == CONSTANTS ==
extern const double G;

// == VARIABLES ==
extern std::vector<DimOrbit::GravityBody*> bodyList;

// == FUNCTIONS ==
void registerBody(DimOrbit::GravityBody* body);
void tick(float delta);

} // namespace DimOrbit::gravity