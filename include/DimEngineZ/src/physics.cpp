#include "../include/DimEngineZ/physics.h"

namespace DimEngineZ::physics {
void resolveCollisionForObject(PhysicsObject* object) {
    for (int j = 0; j < objects.size(); j++) {
        if (objects[j] == object)
            continue;
        object->resolveCollision(*objects[j]);
    }
}
void registerObject(PhysicsObject* object) {
    objects.push_back(object);
}
void tick(float delta) {
    for (int i = 0; i < objects.size(); i++) {
        objects[i]->tick(delta);
        resolveCollisionForObject(objects[i]);
    }
}

} // namespace DimEngineZ::physics