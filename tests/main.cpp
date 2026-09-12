#include <DimOrbit/DimOrbit.h>
#include <cmath>
#include <memory>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    dez::manager::init(1000, 800, "Gravity Simulation");

    auto camera = dez::Camera({-10.0f, 0.0f, 0.0f}, dez::CameraOptions{
                                                        .fovy = 90.0f,
                                                        .direction = {1.0f, 0.0f, 0.0f},
                                                    });
    camera.setTarget(dez::Vec3{0.0f, 0.0f, 0.0f});

    auto system = dor::CelestialSystem();
    system.enableXZClue(true);
    system.setMainCamera(camera);

    auto& sun = system.addBody({.radius = 1.0f, .color = YELLOW, .mass = 2.0, .collide = false});
    auto& earth = system.addBody(dor::BodyOptions{.radius = 0.2f,
                                                  .color = GREEN,
                                                  .position = dez::Vec3{-8.0f, 0.0f, 0.0f},
                                                  .velocity = dez::Vec3{0.0f, 0.0f, 2.2f},
                                                  .mass = 6e-6,
                                                  .collide = false});

    constexpr float CAM_SPEED = 5.0f;
    constexpr float CAM_VERTICAL_SPEED = 50.0f;
    constexpr float LOOK_SPEED = 1.8f;

    float cameraYaw = std::atan2(camera.direction.z, camera.direction.x);
    float cameraPitch = std::asin(camera.direction.y / Vector3Length(camera.direction));

    float outerDelta = 0.0f;
    return dez::manager::fixedloop(
        60,

        // PHYSICS
        [&](float delta) {
            outerDelta = delta;
            system.tick(delta);

            return true;
        },

        // RENDER
        [&]() {
            BeginDrawing();
            cameraYaw += dez::input::getAxis(KEY_LEFT, KEY_RIGHT) * LOOK_SPEED * outerDelta;
            cameraPitch += dez::input::getAxis(KEY_DOWN, KEY_UP) * LOOK_SPEED * outerDelta;
            cameraPitch = Clamp(cameraPitch, -1.5707f, 1.5707f);

            camera.direction = Vector3{
                std::cos(cameraPitch) * std::cos(cameraYaw),
                std::sin(cameraPitch),
                std::cos(cameraPitch) * std::sin(cameraYaw),
            };
            camera.setTarget(camera.position + camera.direction);

            Vector3 forward =
                Vector3Normalize(Vector3{camera.direction.x, 0.0f, camera.direction.z});
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
            Vector3 movement = Vector3Add(Vector3Scale(forward, dez::input::getAxis(KEY_S, KEY_W)),
                                          Vector3Scale(right, dez::input::getAxis(KEY_A, KEY_D)));
            camera.move(
                Vector3Scale(movement + Vector3{0.0f,
                                                dez::input::getAxis(KEY_LEFT_SHIFT, KEY_SPACE) *
                                                    CAM_VERTICAL_SPEED * outerDelta,
                                                0.0f},
                             CAM_SPEED * outerDelta));

            system.render();
            BeginMode3D(camera);

            EndMode3D();

            EndDrawing();

            return true;
        });
}