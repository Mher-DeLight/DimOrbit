#include <DimOrbit/DimOrbit.h>
#include <memory>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    dez::manager::init(1000, 800, "Gravity Simulation");

    auto system = dor::CelestialSystem();
    auto renderer = dor::Renderer();
    renderer.enableXZClue(true);

    auto camera = dez::Camera({0.0f, 0.0f, -5.0f}, dez::CameraOptions{
                                                       .fovy = 90.0f,
                                                       .direction = {1.0f, 0.0f, 0.0f},
                                                   });
    camera.setTarget(dez::Vec3{0.0f, 0.0f, 0.0f});

    auto sun = system.addBody(dor::BodyOptions{.name = "Sun",
                                               .radius = 1.0f,
                                               .color = YELLOW,
                                               .velocity = dez::Vec3::UP,
                                               .mass = 2.0,
                                               .collide = false});

    auto earth = system.addBody(dor::BodyOptions{.name = "Earth",
                                                 .radius = 0.2f,
                                                 .color = GREEN,
                                                 .position = dez::Vec3{-8.0f, 0.0f, 0.0f},
                                                 .velocity = dez::Vec3{0.0f, 0.0f, 2.2f},
                                                 .mass = 6e-6,
                                                 .collide = false});

    auto spacecraft =
        system.addSpacecraft(dor::BasicSpacecraftOptions{.name = "Explorer",
                                                         .radius = 0.1f,
                                                         .color = BLUE,
                                                         .position = dez::Vec3{10.0f, 0.0f, 5.0f},
                                                         .velocity = dez::Vec3{-1.0f, 0.0f, -1.0f},
                                                         .mass = 1e-12,
                                                         .collide = false});
    renderer.addAttribute(*spacecraft->body.get(), dor::RENATR_SHOW_NAME);

    constexpr float SPACECRAFT_SPEED = 1e-12f;

    Vector3 cameraOffset = camera.position;
    constexpr float CAMERA_LOOK_SPEED = 90.0f;
    constexpr float CAMERA_MIN_PITCH = -89.0f;
    constexpr float CAMERA_MAX_PITCH = 89.0f;
    constexpr float CAMERA_DISTANCE = 5.0f;

    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

    return dez::manager::fixedloop(
        60,

        // PHYSICS
        [&](float delta) {
            // camera rotation

            const float yawInput = dez::input::getAxis(KEY_LEFT, KEY_RIGHT);
            const float pitchInput = dez::input::getAxis(KEY_DOWN, KEY_UP);

            cameraYaw += yawInput * CAMERA_LOOK_SPEED * delta;
            cameraPitch += pitchInput * CAMERA_LOOK_SPEED * delta;
            cameraPitch = Clamp(cameraPitch, CAMERA_MIN_PITCH, CAMERA_MAX_PITCH);

            const float yawRad = cameraYaw * DEG2RAD;
            const float pitchRad = cameraPitch * DEG2RAD;

            cameraOffset = Vector3{
                CAMERA_DISTANCE * cosf(pitchRad) * sinf(yawRad),
                CAMERA_DISTANCE * sinf(pitchRad),
                -CAMERA_DISTANCE * cosf(pitchRad) * cosf(yawRad),
            };

            // spacecraft controls
            Vector3 forward = Vector3Normalize(camera.direction);
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

            Vector3 thrust = Vector3Add(Vector3Scale(forward, dez::input::getAxis(KEY_S, KEY_W)),
                                        Vector3Scale(right, dez::input::getAxis(KEY_A, KEY_D)));

            thrust = Vector3Add(
                thrust, Vector3Scale(camera.up, dez::input::getAxis(KEY_LEFT_SHIFT, KEY_SPACE)));

            spacecraft->setThrust(dez::Vec3{
                thrust.x * SPACECRAFT_SPEED,
                thrust.y * SPACECRAFT_SPEED,
                thrust.z * SPACECRAFT_SPEED,
            });

            // physics
            system.tick(delta);
            spacecraft->tick(delta);

            return true;
        },

        // RENDER
        [&]() {
            BeginDrawing();

            Vector3 spacecraftPosition = spacecraft->body->physics->transform.position;

            camera.position = Vector3Add(spacecraftPosition, cameraOffset);

            camera.direction =
                Vector3Normalize(Vector3Subtract(spacecraftPosition, camera.position));

            camera.setTarget(spacecraftPosition);

            renderer.renderCS(system, camera);

            EndDrawing();

            return true;
        });
}