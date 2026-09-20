#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    // setup
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    using Vec3 = dez::Vec3;
    dez::manager::init(1000, 800, "Newtonian Gravity Simulation");

    // initialization
    auto system = dor::CelestialSystem();

    auto renderer = dor::Renderer();
    renderer.enableXZClue(true);
    renderer.xzclue.spacing = 2.0f;
    renderer.xzclue.slices = 20;

    auto clock = dor::Clock(system);

    auto camera = dez::Camera({-5.0f, 5.0f, 5.0f});
    camera.setTarget({0.0f, 0.0f, 0.0f});

    auto controller = dor::Controller();
    controller.bindVec3("move", KEY_W, KEY_S, KEY_A, KEY_D, KEY_E, KEY_Q);
    controller.bindVec2("look", KEY_T, KEY_G, KEY_F, KEY_H);
    controller.bindVec3("thrust", KEY_RIGHT_SHIFT, KEY_KP_1, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP);

    // objects
    auto sun = system.addBody(dor::BodyOptions{
        .name = "Sun",
        .radius = 5.0f,
        .color = YELLOW,
        .mass = 1.0f,
    });
    auto explorer = system.addSpacecraft(dor::BasicSpacecraftOptions{
        .name = "Explorer",
        .radius = 0.1f,
        .color = WHITE,
        .position = Vec3{20.0f, 0.0f, 0.0f},
        .mass = 1e-9,
        .specificImpulse = 1e-9,
    });
    explorer->engine.start(0.9);
    explorer->body->beginOrbit(*sun.get(), 5.0f, 0.0f);
    explorer->createTimeManeuver(dor::Time(5000), dor::maneuver::DeltaV({0.0f, 0.0f, -5e-9f}),
                                 clock);
    clock.doAt(4998, [&]() { controller.disable(); });
    clock.doAt(6000, [&]() { controller.enable(); });

    constexpr float CAM_SPEED = 15.0f;
    constexpr float CAM_LOOK_SPEED = 2.0f;

    return dez::manager::fixedloop(
        60, 60,
        [&](float delta) {
            auto moveVec = controller.getVec3("move");
            camera.moveRight(moveVec.x * CAM_SPEED * delta);
            camera.moveForward(moveVec.y * CAM_SPEED * delta);
            camera.moveUp(moveVec.z * CAM_SPEED * delta);

            auto lookVec = controller.getVec2("look");
            camera.lookAround(-lookVec.x * CAM_LOOK_SPEED * delta,
                              lookVec.y * CAM_LOOK_SPEED * delta);

            auto inVec = controller.getVec3("thrust");

            if (controller.enabled())
                explorer->engine.setMaxThrust(inVec * 1e-7 * delta);

            clock.tick(delta);
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

                renderer.renderName(*explorer.get(), RED);
                renderer.renderName(*sun.get(), GREEN);

                renderer.draw2DLabel(clock.time.getUTCTime(), dez::Vec2{0.0f, 0.0f});
                renderer.draw2DLabel("Fuel: " + std::to_string(explorer->fuelTank.fuel()),
                                     dez::Vec2{0.0f, 30.0f});
                return 0;
            });

            return true;
        });
}