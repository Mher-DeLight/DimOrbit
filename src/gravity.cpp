#include "../include/DimOrbit/gravity.h"

namespace DimOrbit::gravity {
// == CONSTANTS ==
const double G = 6.67430e-11; // N*(m^2)/(kg^2)

// == VARIABLES ==
std::vector<DimOrbit::GravityBody*> bodyList;

// == FUNCTIONS ==
void registerBody(DimOrbit::GravityBody* body) {
    bodyList.push_back(body);
}
void tick(float delta) {
    for (auto* i : bodyList) {
        for (auto* j : bodyList) {
            if (i == j)
                continue;

            i->gravitate_Newtonian(j, delta);
        }
    }
}

} // namespace DimOrbit::gravity