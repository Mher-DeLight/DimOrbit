#include "../include/DimOrbit/DimOrbit.h"
#include "../include/DimOrbit/gravity.h"
#include <iostream>

namespace DimOrbit {

// == GRAVITY BODY ==
void GravityBody::gravitate_Newtonian(GravityBody* other, float delta) {
    dez::Vec3 diff = other->physics->transform.position - physics->transform.position;
    float dist = diff.magnitude();

    physics->core.applyAcceleration(gravity::G * physics->core.mass * diff / (dist * dist * dist),
                                    delta);
}
GravityBody::GravityBody(uq<dez::PhysicsObject> physics_) : physics(std::move(physics)) {
    gravity::registerBody(this);
}

} // namespace DimOrbit