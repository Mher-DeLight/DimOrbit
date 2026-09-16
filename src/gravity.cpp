#include "../include/DimOrbit/gravity.h"

namespace DimOrbit::gravity {
// == CONSTANTS ==
const double G = 39.4784; // N*(m^2)/(kg^2)

// == VARIABLES ==
std::vector<DimOrbit::CelestialBody*> bodyList;

// == FUNCTIONS ==
void registerBody(DimOrbit::CelestialBody* body) {
    bodyList.push_back(body);
}
void tick(float delta) {
    for (auto* i : bodyList) {
        DimOrbit::Vec3 acceleration = DimOrbit::Vec3::ZERO;
        for (auto* j : bodyList) {
            if (i == j)
                continue;

            acceleration += i->gravity.gravitateNewtonian(i->physics->transform.position,
                                                          j->physics->transform.position,
                                                          j->physics->core.mass);
        }
        i->gravity.lastAcceleration = acceleration;
        i->tick(delta);
    }
}

} // namespace DimOrbit::gravity