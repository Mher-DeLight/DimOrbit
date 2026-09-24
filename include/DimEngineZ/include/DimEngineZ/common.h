#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <variant>
#include <vector>

namespace DimEngineZ {

struct Vec3 {
    double x;
    double y;
    double z;

    Vec3(double x_ = 0.0, double y_ = 0.0, double z_ = 0.0) : x(x_), y(y_), z(z_) {}
    Vec3(const Vector3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
    Vec3(std::initializer_list<double> lst)
        : x(*(lst.begin() + 0)), y(*(lst.begin() + 1)), z(*(lst.begin() + 2)) {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

    double sqr_magnitude() const;
    double magnitude() const;
    Vec3 normalized() const;
    Vec3 relativeTo(const Vec3& other) const;
    Vec3 to(const Vec3& other) const;
    Vec3 directionTo(const Vec3& other) const;
    double distanceTo(const Vec3& other) const;

    bool isZero() const;

    Vec3 operator+(const Vec3& other) const {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    Vec3 operator/(const Vec3& other) const {
        return Vec3(x / other.x, y * other.y, z * other.z);
    }
    Vec3 operator*(double scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    Vec3 operator*(int scalar) const {
        float sc = scalar;
        return Vec3(x * sc, y * sc, z * sc);
    }
    Vec3 operator/(double scalar) const {
        double inv = 1.0 / scalar;
        return (*this) * inv;
    }

    Vec3& operator=(const Vec3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    Vec3 operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Vec3 operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Vec3 operator*=(const Vec3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    Vec3 operator/=(const Vec3& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    bool operator==(const Vec3& other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
    operator bool() const {
        return !isZero();
    }
    bool operator!() const {
        return isZero();
    }

    operator Vector3() const {
#define fl(x) static_cast<float>(x)
        return Vector3{fl(x), fl(y), fl(z)};
#undef fl
    }
    operator std::string() const {
        return "Vec3{x=" + std::to_string(x) + ", y=" + std::to_string(y) +
               ", z=" + std::to_string(z) + "}";
    }

    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 LEFT;
    static const Vec3 RIGHT;
    static const Vec3 FORWARD;
    static const Vec3 BACKWARD;
};
struct Vec2 {
private:
    Vector2 vec;

public:
    float& x;
    float& y;

    Vec2(float x_ = 0.0f, float y_ = 0.0f) : vec{x_, y_}, x(vec.x), y(vec.y) {}
    Vec2(const Vector2& initvec) : vec(initvec), x(vec.x), y(vec.y) {}
    Vec2(std::initializer_list<float> lst)
        : vec{*(lst.begin()), *(lst.begin() + 1)}, x(vec.x), y(vec.y) {}
    Vec2(const Vec2& other) : vec(other.vec), x(vec.x), y(vec.y) {}

    float magnitude() const;
    Vector2 normalize() const;

    Vec2 operator+(const Vec2& other) {
        return Vec2(other.vec + vec);
    }
    Vec2 operator-(const Vec2& other) {
        return Vec2(other.vec - vec);
    }
    Vec2 operator*(const Vec2& other) {
        return Vec2(other.vec * vec);
    }
    Vec2 operator/(const Vec2& other) {
        return Vec2(other.vec / vec);
    }
    Vec2 operator*(float scalar) {
        return Vec2(vec * scalar);
    }
    Vec2 operator/(float scalar) {
        return Vec2(vec / scalar);
    }

    Vec2& operator=(const Vec2& other) {
        vec = other.vec;
        return *this;
    }
    Vec2 operator+=(const Vec2& other) {
        vec += other.vec;
        return *this;
    }

    operator const Vector2&() const {
        return vec;
    }

    static const Vec2 ZERO;
    static const Vec2 ONE;
    static const Vec2 UP;
    static const Vec2 DOWN;
    static const Vec2 LEFT;
    static const Vec2 RIGHT;
};

struct Transform {
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 rotation{0.0f, 0.0f, 0.0f};
    Vec3 scale{1.0f, 1.0f, 1.0f};

    float magnitude() const;
    float rotationMagnitude() const;

    void rotateX(double amount);
    void rotateY(double amount);
    void rotateZ(double amount);

    void move(const Vector3& displacement);
    void moveX(double amount);
    void moveY(double amount);
    void moveZ(double amount);

    void goTo(const Vector3& position);
    void goToX(double x);
    void goToY(double y);
    void goToZ(double z);

    Transform(Vec3 position_ = {0.0, 0.0, 0.0}, Vec3 rotation_ = {0.0, 0.0, 0.0},
              Vec3 scale_ = {1.0, 1.0, 1.0})
        : position(position_), rotation(rotation_), scale(scale_) {}
};
struct DrawObject {
    Model model;
    Transform transform;
    Color color;

    DrawObject(const Mesh& mesh, const Transform& transform, Color color = RED)
        : model(LoadModelFromMesh(mesh)), transform(transform), color(color) {
        selfRegister();
    }

    ~DrawObject() {
        if (IsWindowReady())
            UnloadModel(model);
    }

    DrawObject(const DrawObject&) = delete;
    DrawObject& operator=(const DrawObject&) = delete;

    DrawObject(DrawObject&& other) noexcept
        : model(other.model), transform(other.transform), color(other.color) {
        other.model = {}; // to prevent other from unloading the same raylib resources
    }
    DrawObject& operator=(DrawObject&& other) noexcept {
        if (this != &other) {
            UnloadModel(model);

            model = other.model;
            transform = other.transform;
            color = other.color;

            other.model = {};
        }

        return *this;
    }

    void draw() const;

private:
    void selfRegister();
};
struct MovementObject {
    DrawObject shape;

    Vec3 velocity{0.0, 0.0, 0.0};
    double drag = 0.0f;
    double mass = 1.0f;

    Transform& transform() {
        return shape.transform;
    }

    const Transform& transform() const {
        return shape.transform;
    }

    MovementObject(DrawObject shape_) : shape(std::move(shape_)) {}

    void tick(float delta);
    void applyVelocity(const Vec3& applied);
    void setVelocity(const Vec3& newVelocity);
    void setHorizontalVelocity(const Vec2& newVelocity);
    void applyAcceleration(const Vec3& applied, float delta);
    void applyImpulse(const Vec3& impulse);
    void applyForce(const Vec3& force, float delta);
};
struct CollisionBox {
    Vector3 center;
    double radius;
    void update(const Transform& transform);

    bool colliding_with(const CollisionBox& other) const;

    explicit CollisionBox(const BoundingBox& box_)
        : center{(box_.min.x + box_.max.x) * 0.5f, (box_.min.y + box_.max.y) * 0.5f,
                 (box_.min.z + box_.max.z) * 0.5f},
          radius{std::max({box_.max.x - center.x, box_.max.y - center.y, box_.max.z - center.z})} {}
    explicit CollisionBox(const Model& model) : CollisionBox(GetModelBoundingBox(model)) {}
};
struct PhysicsObject {
    CollisionBox collision;
    MovementObject core;
    Transform& transform;
    bool isOnGround = false;
    bool isStatic = false;
    bool collisionsEnabled = true;
    float bounce = 1.0f;

    void resolveCollision(PhysicsObject& other);
    void tick(float delta);
    void enableStatic(bool is_true) {
        isStatic = is_true;
    }
    void enableCollisions(bool is_true) {
        collisionsEnabled = is_true;
    }

    PhysicsObject(DrawObject shape, float bounce_ = 1.0f)
        : collision(shape.model), core(std::move(shape)), bounce(bounce_),
          transform(core.transform()) {
        selfRegister();
    }

private:
    void selfRegister();
};

struct CameraOptions {
    float fovy = 90.0f;
    Vector3 direction = Vector3{0.0f, 0.0f, 0.0f};
    int projection = CAMERA_PERSPECTIVE;
    Vector3 up = Vector3{0.0f, 1.0f, 0.0f};
};

struct Camera {
private:
    Camera3D cam;

public:
    Vector3& position;
    Vector3& up;
    Vector3 direction;
    float fovy = 90.0f;
    int& projection;

    Camera(const Vector3& position_, const CameraOptions& opts = CameraOptions(),
           bool is_main = false)
        : position(cam.position), up(cam.up), fovy(cam.fovy), projection(cam.projection),
          direction(opts.direction) {
        cam.position = position_;
        cam.fovy = opts.fovy;
        cam.target = position_ + opts.direction;
        cam.projection = opts.projection;
        cam.up = opts.up;
        if (is_main)
            makeMain();
    }

    operator const Camera3D&() const {
        return cam;
    }
    operator Camera3D&() {
        return cam;
    }

    void refreshTarget();
    void makeMain();
    void setTarget(const Vector3& position);

    void move(const Vector3& amount);
    void moveX(double amount);
    void moveY(double amount);
    void moveZ(double amount);

    inline void moveForward(double amount) {
        move(Vector3Normalize(direction) * amount);
    }
    inline void moveBackward(double amount) {
        moveForward(-amount);
    }
    inline void moveRight(double amount) {
        const Vector3 forward = Vector3Normalize(direction);
        const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
        move(right * amount);
    }
    inline void moveLeft(double amount) {
        moveRight(-amount);
    }
    inline void moveUp(double amount) {
        const Vector3 forward = Vector3Normalize(direction);
        const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
        const Vector3 cameraUp = Vector3Normalize(Vector3CrossProduct(right, forward));
        move(cameraUp * amount);
    }
    inline void moveDown(double amount) {
        moveUp(-amount);
    }

    inline void lookAround(float yaw, float pitch) {
        constexpr float PITCH_LIMIT = PI / 2.0f - 0.01f;

        const float directionLength = Vector3Length(direction);
        if (directionLength == 0.0f)
            return;

        const Vector3 worldUp = Vector3Normalize(up);
        Vector3 forward = Vector3Normalize(direction);
        forward = Vector3RotateByAxisAngle(forward, worldUp, yaw);

        const float currentPitch =
            std::asin(std::clamp(Vector3DotProduct(forward, worldUp), -1.0f, 1.0f));
        const float targetPitch = std::clamp(currentPitch + pitch, -PITCH_LIMIT, PITCH_LIMIT);
        const Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, worldUp));
        forward = Vector3RotateByAxisAngle(forward, right, targetPitch - currentPitch);

        direction = forward * directionLength;
        refreshTarget();
    }

    void goTo(const Vector3& pos);
    void goToX(double x);
    void goToY(double y);
    void goToZ(double z);

    inline void rotateX(double amount) {
        direction = Vector3Transform(direction, MatrixRotateX(amount));
        refreshTarget();
    }
    inline void rotateY(double amount) {
        direction = Vector3Transform(direction, MatrixRotateY(amount));
        refreshTarget();
    }
    inline void rotateZ(double amount) {
        direction = Vector3Transform(direction, MatrixRotateZ(amount));
        refreshTarget();
    }
    inline void rotate(const Vector3& rotation) {
        rotateX(rotation.x);
        rotateY(rotation.y);
        rotateZ(rotation.z);
    }
};
} // namespace DimEngineZ
