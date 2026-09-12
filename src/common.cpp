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
}

GravityBody& CelestialSystem::addBody(uq<GravityBody> body) {
    bodies.push_back(std::move(body));
    return *bodies.back();
}
GravityBody& CelestialSystem::addBody(uq<dez::PhysicsObject> body, const std::string& name) {
    bodies.push_back(std::make_unique<GravityBody>(std::move(body)));
    bodies.back()->name = name;
    return *bodies.back();
}
GravityBody& CelestialSystem::addBody(const BodyOptions& options) {
    // clang-format off
    auto body = std::make_unique<GravityBody>(std::make_unique<dez::PhysicsObject>(
        dez::DrawObject(
            GenMeshSphere(options.radius, options.rings, options.slices),
            dez::Transform(options.position, options.rotation, options.scale),
            options.color
        ),
        options.bounce
        ), options.name);
    body->physics->enableCollisions(options.collide);
    body->physics->enableStatic(options.isStatic);
    body->physics->core.applyVelocity(options.velocity);
    body->physics->core.mass = options.mass;
    bodies.push_back(std::move(body));
    return *bodies.back();
    // clang-format on
}

// == RENDERER ==
void Renderer::renderCS(const CelestialSystem& csystem, Camera& camera, Color bgColor) const {
    ClearBackground(bgColor);
    BeginMode3D(camera);

    if (isXZClueEnabled()) {
        const float GROUND_SLICES = xzclue.slices;
        const float GROUND_HEIGHT = xzclue.height;
        const float GROUND_SPACING = xzclue.spacing;
        const float GROUND_EXTENT = GROUND_SLICES * GROUND_SPACING * 0.5f;

        for (int slice = 0; slice <= GROUND_SLICES; ++slice) {
            const float offset = -GROUND_EXTENT + slice * GROUND_SPACING;
            DrawLine3D(Vector3{-GROUND_EXTENT, GROUND_HEIGHT, offset},
                       Vector3{GROUND_EXTENT, GROUND_HEIGHT, offset}, DARKGRAY);
            DrawLine3D(Vector3{offset, GROUND_HEIGHT, -GROUND_EXTENT},
                       Vector3{offset, GROUND_HEIGHT, GROUND_EXTENT}, DARKGRAY);
        }
    }

    struct NameLabel {
        std::string text;
        Vector2 position;
    };
    std::vector<NameLabel> nameLabels;

    for (auto& body : csystem.bodies) {
        body->physics->core.shape.draw();

        nameLabels.push_back({
            body->name,
            GetWorldToScreen(Vector3{body->physics->transform.position.x,
                                     body->physics->collision.box.max.y,
                                     body->physics->transform.position.z},
                             camera),
        });
    }

    EndMode3D();

    constexpr Color nameColor = RED;
    for (const auto& label : nameLabels) {
        DrawText(label.text.c_str(), static_cast<int>(label.position.x),
                 static_cast<int>(label.position.y), 20, nameColor);
    }
}
bool Renderer::isXZClueEnabled() const {
    return xzclue.enabled;
}
bool Renderer::enableXZClue(bool is_true) {
    bool old = xzclue.enabled;
    xzclue.enabled = is_true;
    return old;
}

// == GRAVITY BODY ==
void GravityBody::gravitate_Newtonian(GravityBody* other, float delta) {
    dez::Vec3 diff = other->physics->transform.position - physics->transform.position;
    double dist = diff.magnitude();

    if (dist == 0.0f)
        return;

    const double accelerationScale =
        static_cast<double>(gravity::G * other->physics->core.mass / (dist * dist * dist));
    physics->core.applyAcceleration(diff * accelerationScale, delta);
}
GravityBody::GravityBody(uq<dez::PhysicsObject> physics_, const std::string& name_)
    : physics(std::move(physics_)), name(name_) {
    gravity::registerBody(this);
}

} // namespace DimOrbit