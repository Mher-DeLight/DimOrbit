#pragma once
#include <DimEngineZ/DimEngineZ.h>
#include <memory>
#include <optional>

namespace DimOrbit {
namespace dez = DimEngineZ;
template <typename T> using uq = std::unique_ptr<T>;

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

    GravityBody(uq<dez::PhysicsObject> physics_, const std::string& name_ = "");
};
struct CelestialSystem {
    std::vector<uq<GravityBody>> bodies;

    void tick(float delta);

    GravityBody& addBody(uq<GravityBody> body);
    GravityBody& addBody(uq<dez::PhysicsObject> physics, const std::string& name = "");
    GravityBody& addBody(const BodyOptions& options);

    CelestialSystem() = default;
};
struct Renderer {
    XZClue xzclue;

    // XZ Clue
    bool isXZClueEnabled() const;
    bool enableXZClue(bool is_true = true); // returns the old state

    // Render Methods
    void renderCS(const CelestialSystem& csystem, Camera& camera, Color bgColor = BLACK) const;
};

} // namespace DimOrbit