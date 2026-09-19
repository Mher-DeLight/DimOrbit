#pragma once
#include "clock.h"
#include "common.h"
#include "maneuvers.h"
#include <functional>

namespace DimOrbit {
struct BasicSpacecraft {
    uq<CelestialBody> body;
    dez::PhysicsObject* physics;
    FuelTank fuelTank;
    double fuelElapseRate = 1.0f;
    dez::Vec3 thrust = dez::Vec3::ZERO;
    Engine engine;

    // Constructors
    BasicSpacecraft(uq<CelestialBody> body_)
        : body(std::move(body_)), physics(body->physics.get()) {}
    BasicSpacecraft(const BasicSpacecraftOptions& options) {
        body = std::make_unique<CelestialBody>(
            std::make_unique<dez::PhysicsObject>(
                dez::DrawObject(GenMeshSphere(options.radius, options.rings, options.slices),
                                dez::Transform(options.position, options.rotation, options.scale),
                                options.color),
                options.bounce),
            options.name);
        body->physics->enableCollisions(options.collide);
        body->physics->enableStatic(options.isStatic);
        body->physics->core.applyVelocity(options.velocity);
        body->physics->core.mass = options.mass;
        physics = body->physics.get();
        fuelElapseRate = options.fuelElapseRate;
        fuelTank.setMaxFuel(options.maxFuel);
        fuelTank.setFuel(options.fuel);
        fuelTank.clampFuel();
    }
    void tick(float delta);

    // Maneuvers
    maneuver::TimeManeuver createTimeManeuver(const Time& time, const maneuver::DeltaV& deltaV,
                                              Clock& clock);
    maneuver::ArbitraryTimeManeuver
    scheduleAction(const Time& time, const std::function<void()> action, Clock& clock);
};
}; // namespace DimOrbit