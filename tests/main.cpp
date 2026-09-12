#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    using Vec3 = dez::Vec3;

    dez::manager::init(1000, 800, "Newtonian Gravity Simulation");

    auto system = dor::CelestialSystem();
    auto renderer = dor::Renderer();
    renderer.enableXZClue(true);

    auto camera = dez::Camera({-5.0f, 5.0f, 5.0f});
    camera.setTarget({0.0f, 0.0f, 0.0f});

    auto earth = system.addBody(
        dor::BodyOptions{.name = "Earth", .radius = 1.0f, .color = GREEN, .mass = 1.0f});
    auto explorer = system.addSpacecraft(dor::BasicSpacecraftOptions{
        .name = "Explorer",
        .radius = 0.1f,
        .color = WHITE,
        .position = Vec3{5.0f, 0.0f, 0.0f},
        .mass = 1e-3,
    });
    explorer->beginOrbit(*earth.get(), 1.0f, PI / 2);
    renderer.addAttribute(*explorer->body.get(), dor::RENATR_SHOW_NAME);

    constexpr float CAM_SPEED = 5.0f;

    return dez::manager::fixedloop(
        60, 60,
        [&](float delta) {
            camera.moveX(dez::input::getAxis(KEY_A, KEY_D) * CAM_SPEED * delta);
            camera.moveY(dez::input::getAxis(KEY_LEFT_SHIFT, KEY_SPACE) * CAM_SPEED * delta);
            camera.moveZ(-dez::input::getAxis(KEY_S, KEY_W) * CAM_SPEED * delta);

            system.tick(delta);
            return true;
        },
        [&]() {
            BeginDrawing();

            renderer.renderCS(system, camera);

            EndDrawing();
            return true;
        });
}