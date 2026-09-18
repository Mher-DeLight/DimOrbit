#pragma once
#include "common.h"

namespace DimOrbit {
struct XZClue {
    bool enabled = false;
    int slices = 40;
    float height = -1.5f;
    float spacing = 1.0f;
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