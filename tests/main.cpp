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

    auto sun = system.addBody(
        dor::BodyOptions{.name = "Sun", .radius = 5.0f, .color = YELLOW, .mass = 100.0f});
    /* auto earth = system.addBody(
        dor::BodyOptions{.name = "Earth", .radius = 1.0f, .color = GREEN, .mass = 1.0f});
    earth->beginOrbit(*sun.get(), 3.0f, 0.0f); */
    auto explorer = system.addSpacecraft(dor::BasicSpacecraftOptions{
        .name = "Explorer",
        .radius = 0.1f,
        .color = WHITE,
        .position = Vec3{5.0f, 0.0f, 0.0f},
        .mass = 1e-3,
    });
    explorer->engine.start();
    explorer->engine.setThrust(Vec3{3e-2f, 0.0f, 0.0f});
    renderer.addAttribute(*explorer->body.get(), dor::RENATR_SHOW_NAME);

    constexpr float CAM_SPEED = 5.0f;
    constexpr float CAM_LOOK_SPEED = 2.0f;

    return dez::manager::fixedloop(
        60, 60,
        [&](float delta) {
            camera.moveRight(dez::input::getAxis(KEY_A, KEY_D) * CAM_SPEED * delta);
            camera.moveUp(dez::input::getAxis(KEY_LEFT_SHIFT, KEY_SPACE) * CAM_SPEED * delta);
            camera.moveForward(dez::input::getAxis(KEY_S, KEY_W) * CAM_SPEED * delta);

            camera.lookAround(-dez::input::getAxis(KEY_LEFT, KEY_RIGHT) * CAM_LOOK_SPEED * delta,
                              dez::input::getAxis(KEY_DOWN, KEY_UP) * CAM_LOOK_SPEED * delta);

            explorer->tick(delta);
            system.tick(delta);
            return true;
        },
        [&]() {
            renderer.begin(BLACK);

            renderer.begin3d(camera);
            renderer.render(system);
            renderer.displayVector(explorer->physics->core.velocity,
                                   explorer->physics->transform.position);
            renderer.displayVector(explorer->engine.thrust * 10.0f,
                                   explorer->physics->transform.position, RED);
            renderer.end3d();

            renderer.renderLabels(system, RED);

            renderer.end();
            return true;
        });
}