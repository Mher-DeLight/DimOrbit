#include <DimOrbit/DimOrbit.h>
#include <cmath>
#include <memory>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    dez::manager::init(1000, 800, "Welcome");

    auto camera = dez::Camera({-10.0f, 0.0f, 0.0f},
                              dez::CameraOptions{
                                  .fovy = 90.0f,
                              },
                              MAIN_CAMERA);
    camera.setTarget(dez::Vec3{0.0f, 0.0f, 0.0f});

    auto mesh = GenMeshSphere(1.0f, 32, 32);
    auto drawobj = dez::DrawObject(std::move(mesh), dez::Transform(), YELLOW);
    auto physicsObject = std::make_unique<dez::PhysicsObject>(std::move(drawobj));
    auto sun = dor::GravityBody(std::move(physicsObject));
    sun.physics->core.shape.transform.goTo(Vector3{0.0f, 0.0f, 0.0f});
    sun.physics->core.mass = 2;
    sun.physics->enableCollisions(false);

    mesh = GenMeshSphere(0.2f, 32, 32);
    drawobj = dez::DrawObject(std::move(mesh), dez::Transform(), GREEN);
    physicsObject = std::make_unique<dez::PhysicsObject>(std::move(drawobj));
    auto earth = dor::GravityBody(std::move(physicsObject));
    earth.physics->transform.goTo(dez::Vec3{-8.0f, 0.0f, 0.0f});
    earth.physics->core.mass = 6e-6;
    earth.physics->enableCollisions(false);
    earth.physics->core.applyVelocity(dez::Vec3{0.0f, 0.0f, 3.1414});

    constexpr float CAM_SPEED = 5.0f;
    constexpr float CAM_VERTICAL_SPEED = 50.0f;
    constexpr float LOOK_SPEED = 1.8f;
    constexpr int SPEED_SCALE = 1;

    float cameraYaw = std::atan2(camera.direction.z, camera.direction.x);
    float cameraPitch = std::asin(camera.direction.y / Vector3Length(camera.direction));
    return dez::manager::main(60, [&](float delta) {
        if (dez::input::isKeyPressed(KEY_ESCAPE)) {
            exit(0);
        }

        cameraYaw += dez::input::getAxis(KEY_LEFT, KEY_RIGHT) * LOOK_SPEED * delta;
        cameraPitch += dez::input::getAxis(KEY_DOWN, KEY_UP) * LOOK_SPEED * delta;
        cameraPitch = Clamp(cameraPitch, -1.5f, 1.5f);

        camera.direction = Vector3{
            std::cos(cameraPitch) * std::cos(cameraYaw),
            std::sin(cameraPitch),
            std::cos(cameraPitch) * std::sin(cameraYaw),
        };
        camera.setTarget(camera.position + camera.direction);

        Vector3 forward = Vector3Normalize(Vector3{camera.direction.x, 0.0f, camera.direction.z});
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
        Vector3 movement = Vector3Add(Vector3Scale(forward, dez::input::getAxis(KEY_S, KEY_W)),
                                      Vector3Scale(right, dez::input::getAxis(KEY_A, KEY_D)));
        camera.move(Vector3Scale(movement + Vector3{0.0f,
                                                    dez::input::getAxis(KEY_LEFT_SHIFT, KEY_SPACE) *
                                                        CAM_VERTICAL_SPEED * delta,
                                                    0.0f},
                                 CAM_SPEED * delta));

        const float simulationDelta = delta / SPEED_SCALE;
        for (int i = 0; i < SPEED_SCALE; i++) {
            dor::gravity::tick(simulationDelta);
            dez::manager::tick(simulationDelta);
        }
        dez::logger::flushLog("x: " + std::to_string(earth.physics->transform.position.x) +
                              " y: " + std::to_string(earth.physics->transform.position.y) +
                              " z: " + std::to_string(earth.physics->transform.position.z));
        return true;
    });
}