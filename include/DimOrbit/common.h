#pragma once
#include <DimEngineZ/DimEngineZ.h>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace DimOrbit {
namespace dez = DimEngineZ;
template <typename T> using uq = std::unique_ptr<T>;

inline constexpr int RENATR_SHOW_NAME = 1;

class BasicSpacecraft;

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
    double fuel = 100.0f;
};

struct XZClue {
    bool enabled = false;
    int slices = 40;
    float height = -1.5f;
    float spacing = 1.0f;
};

struct GravityBody {
    uq<dez::PhysicsObject> physics;
    std::string name = "";

    void gravitate_Newtonian(GravityBody* other, float delta);

    // Utility
    void beginOrbit(const GravityBody& other, double altitude, double inclination);

    GravityBody(uq<dez::PhysicsObject> physics_, const std::string& name_ = "");
};
struct CelestialSystem {
    std::vector<GravityBody*> bodies;
    std::vector<BasicSpacecraft*> basicSpacecrafts;

    void tick(float delta);

    uq<GravityBody> addBody(uq<GravityBody> body);
    uq<GravityBody> addBody(uq<dez::PhysicsObject> physics, const std::string& name = "");
    uq<GravityBody> addBody(const BodyOptions& options);
    uq<BasicSpacecraft> addSpacecraft(const BasicSpacecraftOptions& options);

    CelestialSystem() = default;
};

// == SPACECRAFT ==
struct Engine {
    bool isStarted = false;
    dez::Vec3 thrust = dez::Vec3::ZERO;

    void start();
    void applyThrust(const dez::Vec3& amount);
    void setThrust(const dez::Vec3& newthrust);
    void turnoff();
};
struct BasicSpacecraft {
    uq<GravityBody> body;
    dez::PhysicsObject* physics;
    double fuel = 100.0f;
    double fuelElapseRate = 1.0f;
    dez::Vec3 thrust = dez::Vec3::ZERO;
    Engine engine;

    // Fuel
    void setFuel(float amount);
    void elapseFuel(float amount);
    void refuel(float amount);

    // Constructors
    BasicSpacecraft(uq<GravityBody> body_) : body(std::move(body_)), physics(body->physics.get()) {}
    BasicSpacecraft(const BasicSpacecraftOptions& options) {
        body = std::make_unique<GravityBody>(
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
        fuel = options.fuel;
    }
    void tick(float delta);
};

struct Renderer {
    XZClue xzclue;
    std::unordered_map<const GravityBody*, std::unordered_set<int>> attributes;
    std::optional<std::reference_wrapper<dez::Camera>> mainCamera;
    bool isMode3D = false;
    bool isDrawingMode = false;

    // Attributes
    void addAttribute(const GravityBody& object, int attribute);
    void clearAttribtues(const GravityBody& object);
    bool hasAttribute(const GravityBody& object, int attribute) const;

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
    void renderLabels(const CelestialSystem& csystem, const Color& color = RED);

    void render(const CelestialSystem& csystem) const;

    void end3d();
    void end();
};

} // namespace DimOrbit