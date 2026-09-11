#pragma once
#include "physics.h"
#include <optional>

namespace DimEngineZ::manager {
inline int exitCode = 0;
inline std::optional<std::reference_wrapper<Camera>> mainCamera;

using managedObject = std::variant<DrawObject*, MovementObject*, PhysicsObject*, Camera*>;
inline std::vector<managedObject> handledObjects;
void init(int width, int height, const std::string& title);

using FuncitonCallback = std::function<bool()>;
int loop(int targetFPS, FuncitonCallback func);
int fixedloop(int targetFPS, float fixed_delta_inverse, std::function<bool(float)> func,
              FuncitonCallback render);
int fixedloop(int targetFPS, std::function<bool(float)> func, FuncitonCallback render);
bool render(Color background, bool clear, FuncitonCallback func);

int main(int targetFPS, std::function<bool(float)> func);

void registerObject(managedObject obj);
void tick(float delta);
void tickObject(managedObject obj, float delta);
void drawObject(managedObject obj);

} // namespace DimEngineZ::manager
