#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    using Vec3 = dez::Vec3;

    dez::manager::init(1000, 800, "Newtonian Gravity Simulation");

    auto system = dor::CelestialSystem();
    auto renderer = dor::Renderer();
    renderer.enableXZClue(true);
    renderer.xzclue.spacing = 2.0f;
    renderer.xzclue.slices = 20;

    auto camera = dez::Camera({-5.0f, 5.0f, 5.0f});
    camera.setTarget({0.0f, 0.0f, 0.0f});

    auto sun = system.addBody(
        dor::BodyOptions{.name = "Sun", .radius = 5.0f, .color = YELLOW, .mass = 1.0f});
    auto explorer = system.addSpacecraft(dor::BasicSpacecraftOptions{
        .name = "Explorer",
        .radius = 0.1f,
        .color = WHITE,
        .position = Vec3{20.0f, 0.0f, 0.0f},
        .mass = 1e-9,
        .fuelElapseRate = 2.5,
    });
    explorer->engine.start();
    explorer->engine.setThrottle(0.9);
    explorer->body->beginOrbit(*sun.get(), 5.0f, 0.0f);

    constexpr float CAM_SPEED = 15.0f;
    constexpr float CAM_LOOK_SPEED = 2.0f;

    return dez::manager::fixedloop(
        60, 60,
        [&](float delta) {
            camera.moveRight(dez::input::getAxis(KEY_A, KEY_D) * CAM_SPEED * delta);
            camera.moveForward(dez::input::getAxis(KEY_S, KEY_W) * CAM_SPEED * delta);

            camera.lookAround(-dez::input::getAxis(KEY_F, KEY_H) * CAM_LOOK_SPEED * delta,
                              dez::input::getAxis(KEY_G, KEY_T) * CAM_LOOK_SPEED * delta);

            system.tick(delta);

            auto inVec = dez::input::getVector3(KEY_LEFT, KEY_RIGHT, KEY_RIGHT_SHIFT, KEY_KP_1,
                                                KEY_DOWN, KEY_UP);
            explorer->engine.setMaxThrust(inVec * 3e-9);
            return true;
        },
        [&]() {
            renderer.doindraw(BLACK, [&]() {
                renderer.doin3d(camera, [&]() {
                    renderer.render(system);
                    renderer.displayVector(explorer->physics->core.velocity,
                                           explorer->physics->transform.position);
                    renderer.displayVector(explorer->engine.maxThrust * 1e9f,
                                           explorer->physics->transform.position, RED);
                    return 0;
                });

                renderer.renderLabel(*explorer.get(), RED);
                return 0;
            });

            return true;
        });
}