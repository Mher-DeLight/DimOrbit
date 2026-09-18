#include "../include/DimOrbit/renderer.h"

namespace DimOrbit {
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
        if (hasAttribute(*body, RENATR::SHOW_NAME)) {
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
        if (hasAttribute(*body, RENATR::SHOW_NAME)) {
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
    renderLabel(body.name,
                Vec3{
                    body.physics->transform.position.x,
                    body.physics->transform.position.y + body.physics->collision.radius,
                    body.physics->transform.position.z,
                },
                color);
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
} // namespace DimOrbit