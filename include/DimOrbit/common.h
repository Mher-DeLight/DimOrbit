#pragma once
#include <DimEngineZ/DimEngineZ.h>
#include <memory>

namespace DimOrbit {
namespace dez = DimEngineZ;
template <typename T> using uq = std::unique_ptr<T>;

struct GravityBody {
    uq<dez::PhysicsObject> physics;

    void gravitate_Newtonian(GravityBody* other, float delta);

    GravityBody(uq<dez::PhysicsObject> physics_);
};
struct CelestialSystem {
    std::vector<uq<GravityBody>> bodies;

    void tick(float delta);
    GravityBody& addBody(uq<GravityBody> body);
    GravityBody& addBody(uq<dez::PhysicsObject> physics);

    CelestialSystem() = default;
};

} // namespace DimOrbit