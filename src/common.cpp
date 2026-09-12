#include "../include/DimOrbit/common.h"
#include "../include/DimOrbit/gravity.h"
#include <iostream>

namespace DimOrbit {

// == CELESTIAL SYSTEM ==
void CelestialSystem::tick(float delta) {
    for (auto& i : bodies) {
        for (auto& j : bodies) {
            if (i.get() == j.get())
                continue;

            i->gravitate_Newtonian(j.get(), delta);
        }
    }

    for (auto& obj : bodies) {
        dez::manager::tickObject(obj->physics.get(), delta);
    }

    ClearBackground(RAYWHITE);
    if (!dez::manager::mainCamera.has_value()) {
        throw std::runtime_error("DimOrbit: invalid render cycle; no main camera assigned");
    }
    BeginMode3D(dez::manager::mainCamera->get());
    for (auto& obj : bodies) {
        dez::manager::drawObject(obj->physics.get());
    }
    EndMode3D();
}
GravityBody& CelestialSystem::addBody(uq<GravityBody> body) {
    bodies.push_back(std::move(body));
    return *bodies.back();
}
GravityBody& CelestialSystem::addBody(uq<dez::PhysicsObject> body) {
    bodies.push_back(std::make_unique<GravityBody>(std::move(body)));
    return *bodies.back();
}

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