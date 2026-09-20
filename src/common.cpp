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
        static_cast<double>(math::G * massOther / (dist * dist * dist));
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
void CelestialBody::beginCircularOrbit(const CelestialBody& other, double altitude,
                                       double inclination, double completeness) {
    const auto& otherPhysics = other.physics->core;
    const double radius = static_cast<double>(other.physics->collision.radius) + altitude;
    const double speed = std::sqrt(math::G * otherPhysics.mass / radius);

    const float i = static_cast<float>(inclination);
    const float a = static_cast<float>(completeness);

    const float sinI = std::sin(i);
    const float cosI = std::cos(i);
    const float sinA = std::sin(a);
    const float cosA = std::cos(a);

    const Vec3 radial{cosA, sinA * sinI, sinA * cosI};
    const Vec3 tangent{-sinA, cosA * sinI, cosA * cosI};

    physics->transform.position =
        otherPhysics.transform().position + radial * static_cast<float>(radius);

    physics->core.setVelocity(otherPhysics.velocity + tangent * static_cast<float>(speed));
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

double Engine::idealExhaustVelocity() const {
    return specificImpulse * math::g0;
}
double Engine::effectiveExhaustVelocity() const {
    return idealExhaustVelocity(); // currently assuming ideal conditions, might change later
}
double Engine::propellantFlowRate() const {
    if (!isStarted)
        return 0.0;

    double amount = thrust().magnitude() / effectiveExhaustVelocity();
    return amount;
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