#pragma once
#include "common.h"

namespace DimEngineZ::physics {
inline std::vector<PhysicsObject*> objects;
void resolveCollisionForObject(PhysicsObject* object);
void registerObject(PhysicsObject* object);
void tick(float delta);
} // namespace DimEngineZ::physics
