#pragma once
#include <functional>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <variant>
#include <vector>

namespace DimEngineZ {

struct Vec3 {
private:
    Vector3 vec;

public:
    float& x;
    float& y;
    float& z;

    Vec3(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f)
        : vec{x_, y_, z_}, x(vec.x), y(vec.y), z(vec.z) {}
    Vec3(const Vector3& initvec) : vec(initvec), x(vec.x), y(vec.y), z(vec.z) {}
    Vec3(std::initializer_list<float> lst)
        : vec{*(lst.begin()), *(lst.begin() + 1), *(lst.begin() + 2)}, x(vec.x), y(vec.y),
          z(vec.z) {}
    Vec3(const Vec3& other) : vec(other.vec), x(vec.x), y(vec.y), z(vec.z) {}

    float magnitude() const;
    Vector3 normalize() const;

    Vec3 operator+(const Vec3& other) {
        return Vec3(other.vec + vec);
    }
    Vec3 operator-(const Vec3& other) {
        return Vec3(other.vec - vec);
    }
    Vec3 operator*(const Vec3& other) {
        return Vec3(other.vec * vec);
    }
    Vec3 operator/(const Vec3& other) {
        return Vec3(other.vec / vec);
    }
    Vec3 operator*(float scalar) {
        return Vec3(vec * scalar);
    }
    Vec3 operator/(float scalar) {
        return Vec3(vec / scalar);
    }

    Vec3& operator=(const Vec3& other) {
        vec = other.vec;
        return *this;
    }
    Vec3 operator+=(const Vec3& other) {
        vec += other.vec;
        return *this;
    }

    operator const Vector3&() const {
        return vec;
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

    void rotateX(float amount);
    void rotateY(float amount);
    void rotateZ(float amount);

    void move(const Vector3& displacement);
    void moveX(float amount);
    void moveY(float amount);
    void moveZ(float amount);

    void goTo(const Vector3& position);
    void goToX(float x);
    void goToY(float y);
    void goToZ(float z);

    Transform(Vector3 position_ = {0.0f, 0.0f, 0.0f}, Vector3 rotation_ = {0.0f, 0.0f, 0.0f},
              Vector3 scale_ = {1.0f, 1.0f, 1.0f})
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

    Vec3 velocity{0, 0, 0};
    float drag = 0.0f;
    float mass = 1.0f;

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
    BoundingBox box;
    void update(const Transform& transform);

    bool colliding_with(const CollisionBox& other) const;

    explicit CollisionBox(const BoundingBox& box_) : box(box_) {}
    explicit CollisionBox(const Model& model) : box(GetModelBoundingBox(model)) {}
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
    void moveX(float amount);
    void moveY(float amount);
    void moveZ(float amount);

    void goTo(const Vector3& pos);
    void goToX(float x);
    void goToY(float y);
    void goToZ(float z);
};
} // namespace DimEngineZ
