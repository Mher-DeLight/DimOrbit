#include "../include/DimEngineZ/common.h"
#include "../include/DimEngineZ/manager.h"
#include <iostream>
#include <stdexcept>
#include <variant>

namespace DimEngineZ {

// == VEC3 ==
float Vec3::magnitude() const {
    return Vector3Length(vec);
}
Vector3 Vec3::normalize() const {
    return Vector3Normalize(vec);
}
const Vec3 Vec3::ZERO{0.0f, 0.0f, 0.0f};
const Vec3 Vec3::ONE{1.0f, 1.0f, 1.0f};
const Vec3 Vec3::UP{0.0f, 1.0f, 0.0f};
const Vec3 Vec3::DOWN{0.0f, -1.0f, 0.0f};
const Vec3 Vec3::LEFT{-1.0f, 0.0f, 0.0f};
const Vec3 Vec3::RIGHT{1.0f, 0.0f, 0.0f};
const Vec3 Vec3::FORWARD{0.0f, 0.0f, 1.0f};
const Vec3 Vec3::BACKWARD{0.0f, 0.0f, -1.0f};

// == VEC2 ==
float Vec2::magnitude() const {
    return Vector2Length(vec);
}
Vector2 Vec2::normalize() const {
    return Vector2Normalize(vec);
}
const Vec2 Vec2::ZERO{0.0f, 0.0f};
const Vec2 Vec2::ONE{1.0f, 1.0f};
const Vec2 Vec2::UP{0.0f, 1.0f};
const Vec2 Vec2::DOWN{0.0f, -1.0f};
const Vec2 Vec2::LEFT{-1.0f, 0.0f};
const Vec2 Vec2::RIGHT{1.0f, 0.0f};

// == TRANSFORM ==
float Transform::magnitude() const {
    return Vector3Length(position);
}
float Transform::rotationMagnitude() const {
    return Vector3Length(rotation);
}
void Transform::rotateX(float amount) {
    rotation.x += amount;
}
void Transform::rotateY(float amount) {
    rotation.y += amount;
}
void Transform::rotateZ(float amount) {
    rotation.z += amount;
}

void Transform::move(const Vector3& displacement) {
    position += displacement;
}
void Transform::moveX(float amount) {
    position.x += amount;
}
void Transform::moveY(float amount) {
    position.y += amount;
}
void Transform::moveZ(float amount) {
    position.z += amount;
}

void Transform::goTo(const Vector3& newposition) {
    position = newposition;
}
void Transform::goToX(float amount) {
    position.x = amount;
}
void Transform::goToY(float amount) {
    position.y = amount;
}
void Transform::goToZ(float amount) {
    position.z = amount;
}

// == DRAW OBJECT ==
void DrawObject::draw() const {
    float angle = transform.rotationMagnitude();

    Vector3 axis = {0.0f, 1.0f, 0.0f};

    DrawModelEx(model, transform.position, axis, angle, transform.scale, color);
}
void DrawObject::selfRegister() {
    manager::registerObject(manager::managedObject(this));
}

// == MOVE OBJECT ==
void MovementObject::tick(float delta_) {
    transform().position += velocity * delta_;

    auto applyDrag = [this, delta_](float& v) {
        if (v > 0.0f)
            v = std::max(v - drag * delta_, 0.0f);
        else if (v < 0.0f)
            v = std::min(v + drag * delta_, 0.0f);
    };

    applyDrag(velocity.x);
    applyDrag(velocity.y);
    applyDrag(velocity.z);
}
void MovementObject::applyVelocity(const Vec3& applied) {
    velocity += applied;
}
void MovementObject::setVelocity(const Vec3& newvelocity) {
    velocity = newvelocity;
}
void MovementObject::setHorizontalVelocity(const Vec2& newVelocity) {
    velocity.x = newVelocity.x;
    velocity.z = newVelocity.y;
}

void MovementObject::applyForce(const Vec3& force, float delta) {
    velocity += (force / mass) * delta;
}
void MovementObject::applyAcceleration(const Vec3& acceleration, float delta) {
    velocity += acceleration * delta;
}
void MovementObject::applyImpulse(const Vec3& impulse) {
    velocity += impulse / mass;
}

// == COLLISION SPHERE ==
bool CollisionBox::colliding_with(const CollisionBox& other) const {
    return CheckCollisionSpheres(center, radius, other.center, other.radius);
}
void CollisionBox::update(const Transform& transform) {
    center = transform.position;
}

// == PHYSICS OBJECT ==
void PhysicsObject::tick(float delta) {
    isOnGround = false;
    core.tick(delta);

    Vector3 position = core.transform().position;

    collision.center = position;
}
void PhysicsObject::resolveCollision(PhysicsObject& other) {
    collision.update(core.transform());
    other.collision.update(other.core.transform());

    if (!collisionsEnabled || !other.collisionsEnabled)
        return;

    if (!collision.colliding_with(other.collision))
        return;

    Vector3 separation = Vector3Subtract(collision.center, other.collision.center);
    float distance = Vector3Length(separation);
    Vector3 normal =
        distance > 0.0f ? Vector3Scale(separation, 1.0f / distance) : Vector3{0.0f, 1.0f, 0.0f};
    float penetration = collision.radius + other.collision.radius - distance;

    float thisShare = other.isStatic ? 1.0f : 0.5f;
    float otherShare = isStatic ? 1.0f : 0.5f;
    if (!isStatic)
        core.transform().move(Vector3Scale(normal, penetration * thisShare));
    if (!other.isStatic)
        other.core.transform().move(Vector3Scale(normal, -penetration * otherShare));

    if (!isStatic) {
        float velocityAlongNormal = Vector3DotProduct(core.velocity, normal);
        core.velocity = core.velocity - Vec3(normal * ((1.0f + bounce) * velocityAlongNormal));
    }
    if (!other.isStatic) {
        float velocityAlongNormal = Vector3DotProduct(other.core.velocity, normal);
        other.core.velocity =
            other.core.velocity - Vec3(normal * ((1.0f + other.bounce) * velocityAlongNormal));
    }

    if (normal.y > 0.0f)
        isOnGround = true;
    else if (normal.y < 0.0f)
        other.isOnGround = true;
}
void PhysicsObject::selfRegister() {
    manager::registerObject(manager::managedObject(this));
}

// == CAMERA ==
void Camera::refreshTarget() {
    cam.target = cam.position + Vector3Normalize(direction);
}
void Camera::makeMain() {
    manager::mainCamera = *this;
}
void Camera::setTarget(const Vector3& target) {
    cam.target = target;
    direction = target - position;
}

void Camera::move(const Vector3& amount) {
    position += amount;
    refreshTarget();
}
void Camera::moveX(float amount) {
    position.x += amount;
    refreshTarget();
}
void Camera::moveY(float amount) {
    position.y += amount;
    refreshTarget();
}
void Camera::moveZ(float amount) {
    position.z += amount;
    refreshTarget();
}
void Camera::goTo(const Vector3& amount) {
    position = amount;
    refreshTarget();
}
void Camera::goToX(float amount) {
    position.x = amount;
    refreshTarget();
}
void Camera::goToY(float amount) {
    position.y = amount;
    refreshTarget();
}
void Camera::goToZ(float amount) {
    position.z = amount;
    refreshTarget();
}

} // namespace DimEngineZ
