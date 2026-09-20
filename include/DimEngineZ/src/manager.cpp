#include "../include/DimEngineZ/manager.h"
#include "../include/DimEngineZ/logger.h"
#include <optional>
#include <stdexcept>

namespace DimEngineZ::manager {
using FuncitonCallback = std::function<bool()>;
using FixedCallback = std::function<bool(float)>;

void init(int width, int height, const std::string& title) {
    SetTraceLogLevel(logger::raylibLogLevel);
    InitWindow(width, height, title.c_str());
}
int loop(int targetFPS, FuncitonCallback func) {
    SetTargetFPS(targetFPS);
    bool status = true;
    while (!WindowShouldClose() && status) {
        status = func();
    }
    CloseWindow();
    return status ? 0 : -1;
}
int fixedloop(int targetFPS, float fixed_delta_inverse, std::function<bool(float)> physics,
              FuncitonCallback render) {
    SetTargetFPS(targetFPS);

    const float fixed_delta = 1.0f / fixed_delta_inverse;
    float accumulator = 0.0f;

    bool status = true;

    while (!WindowShouldClose() && status) {
        accumulator += GetFrameTime();

        while (accumulator >= fixed_delta) {
            status = physics(fixed_delta);
            accumulator -= fixed_delta;
        }
        if (status)
            status = render();
    }

    CloseWindow();
    return exitCode;
}
int fixedloop(int targetFPS, std::function<bool(float)> physics, FuncitonCallback render) {
    SetTargetFPS(targetFPS);

    const float fixed_delta = 1.0f / targetFPS;
    float accumulator = 0.0f;

    bool status = true;

    while (!WindowShouldClose() && status) {
        accumulator += GetFrameTime();

        while (accumulator >= fixed_delta) {
            status = physics(fixed_delta);
            accumulator -= fixed_delta;
        }
        if (status)
            status = render();
    }

    CloseWindow();
    return exitCode;
}
int main(int targetFPS, std::function<bool(float)> func) {
    SetTargetFPS(targetFPS);

    const float fixed_delta = 1.0f / targetFPS;
    float accumulator = 0.0f;

    bool status = true;

    while (!WindowShouldClose() && status) {
        BeginDrawing();
        accumulator += GetFrameTime();

        while (accumulator >= fixed_delta) {
            status = func(fixed_delta);
            accumulator -= fixed_delta;
        }
        EndDrawing();
    }

    CloseWindow();
    return exitCode;
}

bool render(Color background, bool clear, FuncitonCallback func) {
    BeginDrawing();
    if (clear)
        ClearBackground(background);

    bool code = func();
    EndDrawing();
    return code;
}

void registerObject(managedObject obj) {
    handledObjects.push_back(obj);
    if (std::holds_alternative<PhysicsObject*>(obj)) {
        // register at the physics registry too
        physics::registerObject(std::get<PhysicsObject*>(obj));
    }
}
void tick(float delta) {
    for (auto& obj : handledObjects) {
        tickObject(obj, delta);
    }

    ClearBackground(RAYWHITE);
    if (!mainCamera.has_value()) {
        throw std::runtime_error("DimEngineZ: invalid render cycle; no main camera assigned");
    }
    BeginMode3D(mainCamera->get());
    for (auto& obj : handledObjects) {
        drawObject(obj);
    }
    EndMode3D();
}
void tickObject(managedObject obj, float delta) {
    if (std::holds_alternative<PhysicsObject*>(obj)) {
        auto cst = std::get<PhysicsObject*>(obj);
        cst->tick(delta);
        physics::resolveCollisionForObject(cst);
    } else if (std::holds_alternative<DrawObject*>(obj)) {
        // draws are handled separately
    } else if (std::holds_alternative<MovementObject*>(obj)) {
        auto cst = std::get<MovementObject*>(obj);
    } else {
        throw std::runtime_error("DimEngineZ: invalid type in handledObjects for object handler");
    }
}
void drawObject(managedObject obj) {
    if (std::holds_alternative<PhysicsObject*>(obj)) {
        auto cst = std::get<PhysicsObject*>(obj);
        drawObject(managedObject(&cst->core));
    } else if (std::holds_alternative<DrawObject*>(obj)) {
        auto cst = std::get<DrawObject*>(obj);
        cst->draw();
    } else if (std::holds_alternative<MovementObject*>(obj)) {
        auto cst = std::get<MovementObject*>(obj);
        drawObject(managedObject(&cst->shape));
    } else {
        throw std::runtime_error("DimEngineZ: invalid type in handledObjects for object handler");
    }
}

} // namespace DimEngineZ::manager
