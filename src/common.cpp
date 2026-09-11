#include "../include/DimOrbit/common.h"
#include "../include/DimOrbit/gravity.h"
#include <iostream>

namespace DimOrbit {

// == GRAVITY BODY ==
void GravityBody::gravitate_Newtonian(GravityBody* other, float delta) {
    dez::Vec3 diff = other->physics->transform.position - physics->transform.position;
    float dist = diff.magnitude();

    if (dist == 0.0f)
        return;

    physics->core.applyAcceleration(
        gravity::G * other->physics->core.mass * diff / (dist * dist * dist), delta);
}
GravityBody::GravityBody(uq<dez::PhysicsObject> physics_) : physics(std::move(physics_)) {
    gravity::registerBody(this);
}

} // namespace DimOrbit