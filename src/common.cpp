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

    const float accelerationScale =
        static_cast<float>(gravity::G * other->physics->core.mass / (dist * dist * dist));
    physics->core.applyAcceleration(diff * accelerationScale, delta);
}
GravityBody::GravityBody(uq<dez::PhysicsObject> physics_) : physics(std::move(physics_)) {
    gravity::registerBody(this);
}

} // namespace DimOrbit