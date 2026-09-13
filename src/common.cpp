#include "../include/DimOrbit/common.h"
#include "../include/DimOrbit/gravity.h"
#include <cmath>
#include <iostream>

namespace DimOrbit {

// == CELESTIAL SYSTEM ==
void CelestialSystem::tick(float delta) {
    for (auto& i : bodies) {
        for (auto& j : bodies) {
            if (i == j)
                continue;

            i->gravitate_Newtonian(j, delta);
        }
    }

    for (auto& obj : bodies) {
        dez::manager::tickObject(dez::manager::managedObject(obj->physics.get()), delta);
    }
}

uq<GravityBody> CelestialSystem::addBody(uq<GravityBody> body) {
    bodies.push_back(body.get());
    return body;
}
uq<GravityBody> CelestialSystem::addBody(uq<dez::PhysicsObject> body, const std::string& name) {
    auto newbody = std::make_unique<GravityBody>(std::move(body));
    newbody->name = name;
    bodies.push_back(newbody.get());
    return newbody;
}
uq<GravityBody> CelestialSystem::addBody(const BodyOptions& options) {
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
    bodies.push_back(body.get());
    return body;
    // clang-format on
}
uq<BasicSpacecraft> CelestialSystem::addSpacecraft(const BasicSpacecraftOptions& options) {
    auto spacecraft = std::make_unique<BasicSpacecraft>(options);
    bodies.push_back(spacecraft->body.get());
    return spacecraft;
}

// == RENDERER ==
void Renderer::begin(const Color& bgColor) {
    BeginDrawing();
    isDrawingMode = true;
    ClearBackground(bgColor);
}
void Renderer::begin3d(dez::Camera& camera) {
    BeginMode3D(camera);
    isMode3D = true;
    mainCamera = camera;
}
void Renderer::end3d() {
    EndMode3D();
    isMode3D = false;
}
void Renderer::end() {
    if (isMode3D) {
        isMode3D = false;
        EndMode3D();
    }
    EndDrawing();
    isDrawingMode = false;
    mainCamera.reset();
}

int Renderer::doindraw(const Color& bgColor, std::function<int()> func) {
    begin(bgColor);
    int code = func();
    end();
    return code;
}
int Renderer::doin3d(dez::Camera& camera, std::function<int()> func) {
    begin3d(camera);
    int code = func();
    end3d();
    return code;
}

void Renderer::displayVector(const dez::Vec3& vector, const dez::Vec3& origin, const Color& color) {
    DrawLine3D(origin, origin + vector, color);
}
void Renderer::renderLabels(const CelestialSystem& csystem, const Color& color) {
    if (!mainCamera.has_value())
        throw std::runtime_error("DimOrbit: cannot render labels without camera");

    struct NameLabel {
        std::string text;
        Vector2 position;
    };

    std::vector<NameLabel> nameLabels;

    for (auto& body : csystem.bodies) {
        if (hasAttribute(*body, RENATR_SHOW_NAME)) {
            nameLabels.push_back({
                body->name,
                GetWorldToScreen(
                    Vector3{body->physics->transform.position.x,
                            body->physics->collision.center.y + body->physics->collision.radius,
                            body->physics->transform.position.z},
                    mainCamera.value().get()),
            });
        }
    }

    for (const auto& label : nameLabels) {
        DrawText(label.text.c_str(), static_cast<int>(label.position.x),
                 static_cast<int>(label.position.y), 20, color);
    }
}

void Renderer::render(const CelestialSystem& csystem) const {
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

        DrawLine3D(Vector3{-GROUND_EXTENT, GROUND_HEIGHT, 0.0f},
                   Vector3{GROUND_EXTENT, GROUND_HEIGHT, 0.0f}, RED);
        DrawLine3D(Vector3{0.0f, GROUND_HEIGHT, -GROUND_EXTENT},
                   Vector3{0.0f, GROUND_HEIGHT, GROUND_EXTENT}, BLUE);
    }

    for (auto& body : csystem.bodies) {
        body->physics->core.shape.draw();
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
void Renderer::addAttribute(const GravityBody& object, int attribute) {
    attributes[&object].insert(attribute);
}
void Renderer::clearAttribtues(const GravityBody& object) {
    attributes[&object].clear();
}
bool Renderer::hasAttribute(const GravityBody& object, int attribute) const {
    if (!attributes.contains(&object))
        return false;
    return attributes.at(&object).contains(attribute);
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
void GravityBody::beginOrbit(const GravityBody& other, double altitude, double inclination) {
    const auto& otherPhysics = other.physics->core;
    const double orbitalRadius = static_cast<double>(other.physics->collision.radius) + altitude;
    const double orbitalSpeed = std::sqrt(gravity::G * otherPhysics.mass / orbitalRadius);
    const float radius = static_cast<float>(orbitalRadius);
    const float angle = static_cast<float>(inclination);

    physics->transform.position = otherPhysics.transform().position + dez::Vec3{radius, 0.0f, 0.0f};
    physics->core.setVelocity(otherPhysics.velocity +
                              dez::Vec3{0.0f, static_cast<float>(orbitalSpeed * std::sin(angle)),
                                        static_cast<float>(orbitalSpeed * std::cos(angle))});
}
GravityBody::GravityBody(uq<dez::PhysicsObject> physics_, const std::string& name_)
    : physics(std::move(physics_)), name(name_) {
    gravity::registerBody(this);
}

// == ENGINE ==
void Engine::start() {
    isStarted = true;
}
void Engine::turnoff() {
    thrust = dez::Vec3::ZERO;
    isStarted = false;
}

void Engine::applyThrust(const dez::Vec3& amount) {
    if (!isStarted)
        return;
    thrust += amount;
}
void Engine::setThrust(const dez::Vec3& amount) {
    if (!isStarted)
        return;
    thrust = amount;
}

// == BASIC SPACECRAFT ==
void BasicSpacecraft::setFuel(float amount) {
    fuel = amount;
}
void BasicSpacecraft::elapseFuel(float amount) {
    fuel -= amount;
}
void BasicSpacecraft::refuel(float amount) {
    fuel += amount;
}

void BasicSpacecraft::tick(float delta) {
    if (engine.thrust.x == 0 && engine.thrust.y == 0 && engine.thrust.z == 0)
        return; // if there is no thrust, movement will be handled by gravitate_Newtonian

    physics->core.applyForce(engine.thrust, delta);
    elapseFuel(fuelElapseRate * delta);
}

} // namespace DimOrbit