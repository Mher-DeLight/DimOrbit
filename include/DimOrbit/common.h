#pragma once
#include <DimEngineZ/DimEngineZ.h>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace DimOrbit {
namespace dez = DimEngineZ;
using Vec3 = dez::Vec3;
template <typename T> using uq = std::unique_ptr<T>;

struct BasicSpacecraft;

// == CALCULATORS ==
struct GravityCalculator {
    Vec3 lastAcceleration = Vec3::ZERO;
    Vec3 gravitateNewtonian(const Vec3& posSelf, const Vec3& posOther, double massOther);
};

struct BodyOptions {
    std::string name = "";
    float radius = 1.0f;
    Color color = RED;
    float bounce = 1.0f;
    dez::Vec3 position = dez::Vec3::ZERO;
    dez::Vec3 scale = dez::Vec3::ONE;
    dez::Vec3 rotation = dez::Vec3::ZERO;
    dez::Vec3 velocity = dez::Vec3::ZERO;
    double mass = 1.0f;
    bool collide = true;
    bool isStatic = false;
    int rings = 32;
    int slices = 32;
};
struct BasicSpacecraftOptions {
    std::string name = "";

    float radius = 1.0f;
    Color color = RED;

    float bounce = 1.0f;

    dez::Vec3 position = dez::Vec3::ZERO;
    dez::Vec3 scale = dez::Vec3::ONE;
    dez::Vec3 rotation = dez::Vec3::ZERO;
    dez::Vec3 velocity = dez::Vec3::ZERO;

    double mass = 1.0f;

    bool collide = true;
    bool isStatic = false;

    int rings = 32;
    int slices = 32;

    double fuel = 100.0;
    double maxFuel = 100.0;
    double specificImpulse = 2.5;
};

struct CelestialBody {
    uq<dez::PhysicsObject> physics;
    GravityCalculator gravity;
    std::string name = "";

    // Utility
    void beginOrbit(const CelestialBody& other, double altitude, double inclination,
                    double completeness = 0.0);

    void tick(float delta);
    CelestialBody(uq<dez::PhysicsObject> physics_, const std::string& name_ = "");
};
struct CelestialSystem {
    std::vector<CelestialBody*> bodies;
    std::vector<BasicSpacecraft*> basicSpacecrafts;

    void tick(float delta);

    uq<CelestialBody> addBody(uq<CelestialBody> body);
    uq<CelestialBody> addBody(uq<dez::PhysicsObject> physics, const std::string& name = "");
    uq<CelestialBody> addBody(const BodyOptions& options);
    uq<BasicSpacecraft> addSpacecraft(const BasicSpacecraftOptions& options);

    CelestialSystem() = default;
};

// == SPACECRAFT COMPONENTS ==
struct Engine {
    bool isStarted = false;
    dez::Vec3 maxThrust = dez::Vec3::ZERO;
    double throttle = 1.0;
    double specificImpulse = 1.0e-9;

    void start(double initthrottle = 1.0);

    // action
    void setThrottle(double newthrottle);
    double getThrottle() const;
    void applyMaxThrust(const dez::Vec3& amount);
    void setMaxThrust(const dez::Vec3& newthrust);

    // getter/internal
    dez::Vec3 thrust() const;
    double idealExhaustVelocity() const;
    double effectiveExhaustVelocity() const;
    double propellantFlowRate() const;

    void stop();
    void restart(double initthrottle = 1.0);
};
struct FuelTank {
    double _fuel = 100.0;
    double _maxFuel = 100.0;

    void consumeFuel(double amount);
    void setFuel(double amount);
    void refuel(double amount);
    void completeRefuel(); // sets fuel = maxFuel

    void setMaxFuel(double amount);
    void changeMaxFuel(double amount);

    void clampFuel();
    void clampFuel(double min, double max);

    double fuel() const;
    double maxFuel() const;
    bool isEmpty() const;
};

} // namespace DimOrbit
namespace DimOrbit::math {
inline constexpr double G = 39.4784;  // N*(m^2)/(kg^2)
inline constexpr double g0 = 9.80665; // s
} // namespace DimOrbit::math
namespace DimOrbit::RENATR {
inline constexpr int SHOW_NAME = 1;
}
