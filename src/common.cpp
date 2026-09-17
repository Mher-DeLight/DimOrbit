#include "../include/DimOrbit/common.h"
#include <cmath>
#include <iostream>

namespace DimOrbit {

// == CALCULATORS ==
Vec3 GravityCalculator::gravitateNewtonian(const Vec3& posSelf, const Vec3& posOther,
                                           double massOther) {
    Vec3 diff = posOther - posSelf;
    double dist = diff.magnitude();

    if (dist == 0.0f)
        return Vec3::ZERO;

    const double accelerationScale =
        static_cast<double>(gravity::G * massOther / (dist * dist * dist));
    lastAcceleration = diff * accelerationScale;
    return lastAcceleration;
}

// == CELESTIAL SYSTEM ==
void CelestialSystem::tick(float delta) {
    std::vector<CelestialBody*> allBodies = bodies;
    for (auto& i : basicSpacecrafts) {
        i->tick(delta);
        allBodies.push_back(i->body.get());
    }

    for (auto* i : allBodies) {
        Vec3 acceleration = Vec3::ZERO;
        for (auto* j : allBodies) {
            if (i == j)
                continue;

            acceleration += i->gravity.gravitateNewtonian(i->physics->transform.position,
                                                          j->physics->transform.position,
                                                          j->physics->core.mass);
        }
        i->gravity.lastAcceleration = acceleration;
    }

    for (auto* body : allBodies) {
        body->tick(delta);
    }
}

uq<CelestialBody> CelestialSystem::addBody(uq<CelestialBody> body) {
    bodies.push_back(body.get());
    return body;
}
uq<CelestialBody> CelestialSystem::addBody(uq<dez::PhysicsObject> body, const std::string& name) {
    auto newbody = std::make_unique<CelestialBody>(std::move(body));
    newbody->name = name;
    bodies.push_back(newbody.get());
    return newbody;
}
uq<CelestialBody> CelestialSystem::addBody(const BodyOptions& options) {
    // clang-format off
    auto body = std::make_unique<CelestialBody>(std::make_unique<dez::PhysicsObject>(
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
    basicSpacecrafts.push_back(spacecraft.get());
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
void Renderer::renderNames(const CelestialSystem& csystem, const Color& color) {
    if (!mainCamera.has_value())
        throw std::runtime_error("DimOrbit: cannot render labels without camera");

    struct NameLabel {
        std::string text;
        Vector2 position;
    };

    std::vector<NameLabel> nameLabels;

    for (auto& body : csystem.bodies) {
        if (hasAttribute(*body, RENATR_SHOW_NAME)) {
            float dot = Vector3DotProduct(
                Vector3Normalize(Vector3Subtract(mainCamera.value().get().position,
                                                 body->physics->transform.position)),
                Vector3Normalize(mainCamera.value().get().direction));
            if (dot > 0.0f)
                continue;
            renderName(*body, color);
        }
    }
    for (auto& bscpcraft : csystem.basicSpacecrafts) {
        auto& body = bscpcraft->body;
        if (hasAttribute(*body, RENATR_SHOW_NAME)) {
            float dot = Vector3DotProduct(
                Vector3Normalize(Vector3Subtract(mainCamera.value().get().position,
                                                 body->physics->transform.position)),
                Vector3Normalize(mainCamera.value().get().direction));
            if (dot > 0.0f)
                continue;
            renderName(*body.get(), color);
        }
    }
}
void Renderer::renderName(const CelestialBody& body, const Color& color) {
    renderLabel(body.name, Vec3{
                               body.physics->transform.position.x,
                               body.physics->transform.position.y + body.physics->collision.radius,
                               body.physics->transform.position.z,
                           });
}
void Renderer::renderName(const BasicSpacecraft& body, const Color& color) {
    renderName(*body.body.get(), color);
}
void Renderer::renderLabel(const std::string& text, const Vec3& position, const Color& color,
                           int fontSize) {
    using Vec2 = dez::Vec2;
    float dot = Vector3DotProduct(
        Vector3Normalize(Vector3Subtract(mainCamera.value().get().position, position)),
        Vector3Normalize(mainCamera.value().get().direction));
    if (dot > 0.0f)
        return;
    const Vec2 labPosition = GetWorldToScreen(position, mainCamera.value().get());

    DrawText(text.c_str(), static_cast<int>(labPosition.x), static_cast<int>(labPosition.y),
             fontSize, color);
}
void Renderer::draw2DLabel(const std::string& text, const dez::Vec2& position, const Color& color,
                           int fontSize) {
    DrawText(text.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), fontSize,
             color);
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
    for (auto& bspccraft : csystem.basicSpacecrafts) {
        bspccraft->body->physics->core.shape.draw();
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
void Renderer::addAttribute(const CelestialBody& object, int attribute) {
    attributes[&object].insert(attribute);
}
void Renderer::clearAttribtues(const CelestialBody& object) {
    attributes[&object].clear();
}
bool Renderer::hasAttribute(const CelestialBody& object, int attribute) const {
    if (!attributes.contains(&object))
        return false;
    return attributes.at(&object).contains(attribute);
}

// == GRAVITY BODY ==
void CelestialBody::beginOrbit(const CelestialBody& other, double altitude, double inclination,
                               double completeness) {
    const double alteredCompleteness = completeness + (PI);
    const auto& otherPhysics = other.physics->core;
    const double orbitalRadius = static_cast<double>(other.physics->collision.radius) + altitude;
    const double orbitalSpeed = std::sqrt(gravity::G * otherPhysics.mass / orbitalRadius);
    const float radius = static_cast<float>(orbitalRadius);
    const float angle = static_cast<float>(inclination);
    Vec3& position = physics->transform.position;
    const Vec3 dir = Vector3Normalize(otherPhysics.transform().position - position);
    const Vec3 right = Vector3Normalize(Vector3CrossProduct(dir, Vec3::UP));
    const Vec3 up = Vector3Normalize(Vector3CrossProduct(right, dir));

    position = Vector3RotateByAxisAngle(dir * radius, up, alteredCompleteness);

    physics->core.setVelocity(Vector3RotateByAxisAngle(
        otherPhysics.velocity + dez::Vec3{0.0f, static_cast<float>(orbitalSpeed * std::sin(angle)),
                                          static_cast<float>(orbitalSpeed * std::cos(angle))},
        up, alteredCompleteness));
}
CelestialBody::CelestialBody(uq<dez::PhysicsObject> physics_, const std::string& name_)
    : physics(std::move(physics_)), name(name_) {}
void CelestialBody::tick(float delta) {
    // assume gravity.gravitateNewtonian is already called
    // we can't call it here because we don't have access to the list of all other objects
    physics->core.applyAcceleration(gravity.lastAcceleration, delta);
    dez::manager::tickObject(dez::manager::managedObject(physics.get()), delta);
}

// == ENGINE ==
void Engine::start() {
    isStarted = true;
}
void Engine::stop() {
    maxThrust = dez::Vec3::ZERO;
    isStarted = false;
}

void Engine::applyMaxThrust(const dez::Vec3& amount) {
    if (!isStarted)
        return;
    maxThrust += amount;
}
void Engine::setMaxThrust(const dez::Vec3& amount) {
    if (!isStarted)
        return;
    maxThrust = amount;
}
void Engine::setThrottle(double newthrottle) {
    throttle = newthrottle;
}
double Engine::getThrottle() const {
    return throttle;
}
dez::Vec3 Engine::thrust() const {
    return maxThrust * throttle;
}

// == FUEL TANK ==
void FuelTank::clampFuel() {
    clampFuel(0.0, maxFuel());
}
void FuelTank::clampFuel(double min, double max) {
    if (_fuel < min) {
        _fuel = min;
    }
    if (_fuel > max) {
        _fuel = max;
    }
}
void FuelTank::consumeFuel(double amount) {
    _fuel -= amount;
    clampFuel();
}
void FuelTank::setFuel(double amount) {
    _fuel = amount;
    clampFuel();
}
void FuelTank::refuel(double amount) {
    _fuel += amount;
    clampFuel();
}
void FuelTank::completeRefuel() {
    _fuel = maxFuel();
}

void FuelTank::setMaxFuel(double amount) {
    _maxFuel = amount;
}
void FuelTank::changeMaxFuel(double amount) {
    _maxFuel += amount;
}

double FuelTank::fuel() const {
    return _fuel;
}
double FuelTank::maxFuel() const {
    return _maxFuel;
}
bool FuelTank::isEmpty() const {
    return fuel() <= 0.0;
}

// == BASIC SPACECRAFT ==
void BasicSpacecraft::tick(float delta) {
    if (engine.maxThrust.x == 0 && engine.maxThrust.y == 0 && engine.maxThrust.z == 0)
        return; // if there is no thrust, movement will be handled by gravitate_Newtonian
    if (fuelTank.isEmpty())
        return;

    physics->core.applyForce(engine.thrust(), delta);
    fuelTank.consumeFuel(fuelElapseRate * delta);
}

} // namespace DimOrbit