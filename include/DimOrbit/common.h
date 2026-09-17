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

inline constexpr int RENATR_SHOW_NAME = 1;

class BasicSpacecraft;

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
    double fuelElapseRate = 2.5;
};

struct XZClue {
    bool enabled = false;
    int slices = 40;
    float height = -1.5f;
    float spacing = 1.0f;
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

// == SPACECRAFT ==
struct Engine {
    bool isStarted = false;
    dez::Vec3 maxThrust = dez::Vec3::ZERO;
    double throttle = 1.0;

    void start();

    void setThrottle(double newthrottle);
    double getThrottle() const;
    void applyMaxThrust(const dez::Vec3& amount);
    void setMaxThrust(const dez::Vec3& newthrust);
    dez::Vec3 thrust() const;

    void stop();
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
};

struct Renderer {
    XZClue xzclue;
    std::unordered_map<const CelestialBody*, std::unordered_set<int>> attributes;
    std::optional<std::reference_wrapper<dez::Camera>> mainCamera;
    bool isMode3D = false;
    bool isDrawingMode = false;

    // Attributes
    void addAttribute(const CelestialBody& object, int attribute);
    void clearAttribtues(const CelestialBody& object);
    bool hasAttribute(const CelestialBody& object, int attribute) const;

    // XZ Clue
    bool isXZClueEnabled() const;
    bool enableXZClue(bool is_true = true); // returns the old state

    // Render Methods
    void begin(const Color& bgColor = BLACK);
    void begin3d(dez::Camera& camera);
    int doindraw(const Color& bgColor, std::function<int()> func);
    int doin3d(dez::Camera& camera, std::function<int()> func);

    void displayVector(const dez::Vec3& vector, const dez::Vec3& origin,
                       const Color& color = PURPLE);
    void renderNames(const CelestialSystem& csystem, const Color& color = RED);
    void renderName(const CelestialBody& body, const Color& color = RED);
    void renderName(const BasicSpacecraft& body, const Color& color = RED);
    void renderLabel(const std::string& text, const Vec3& position, const Color& color = RED,
                     int fontSize = 20);
    void draw2DLabel(const std::string& text, const dez::Vec2& position, const Color& color = BLUE,
                     int fontSize = 30);

    void render(const CelestialSystem& csystem) const;

    void end3d();
    void end();
};

} // namespace DimOrbit
namespace DimOrbit::gravity {
inline const double G = 39.4784; // N*(m^2)/(kg^2)
}