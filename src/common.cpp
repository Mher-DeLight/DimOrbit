#include "../include/DimOrbit/common.h"
#include "../include/DimOrbit/spacecraft.h"
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
void Engine::start(double initthrottle) {
    isStarted = true;
    setThrottle(initthrottle);
}
void Engine::stop() {
    maxThrust = dez::Vec3::ZERO;
    isStarted = false;
}
void Engine::restart(double initthrottle) {
    stop();
    start(initthrottle);
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

} // namespace DimOrbit