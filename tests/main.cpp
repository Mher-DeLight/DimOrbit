#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    // aliases
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    using Vec3 = DimEngineZ::Vec3;

    // initialization
    dez::manager::init(1000, 800, "Mission Simulation");

    auto camera = dez::Camera(Vec3{5.0, 2.0, 0.0});
    camera.setTarget(Vec3::ZERO);

    auto system = dor::CelestialSystem();

    auto renderer = dor::Renderer();
    renderer.enableXZClue(true);
    renderer.xzclue.height = 0.0f;

    auto clock = dor::Clock(system);

    // bodies
    auto earth = system.addBody(dor::BodyOptions{
        .name = "Earth",
        .radius = 1.0,
        .color = GREEN,
        .mass = 1.0,
        .collide = false,
    });
    auto spacecraft = system.addSpacecraft(dor::BasicSpacecraftOptions{
        .name = "Spacecraft",
        .radius = 0.1,
        .color = RED,
        .mass = 1e-9,
        .collide = false,
    });
    spacecraft->body->beginCircularOrbit(*earth.get(), 3.0, PI);

    // main loop
    int exit_code = dez::manager::fixedloop(
        60, 60,

        // physics
        [&](float delta) {
            clock.tick(delta);
            return true;
        },

        // rendering
        [&]() {
            renderer.doindraw(BLACK, [&]() {
                renderer.doin3d(camera, [&]() {
                    // render the system in 3d mode
                    renderer.render(system);
                    renderer.displayVector(spacecraft->body->gravity.lastAcceleration,
                                           spacecraft->physics->transform.position);
                    renderer.displayVector(spacecraft->physics->core.velocity,
                                           spacecraft->physics->transform.position, YELLOW);
                    return 0;
                });

                renderer.renderName(*spacecraft.get(), GREEN);
                renderer.draw2DLabel(clock.time.getUTCTime(), dez::Vec2::ZERO);
                return 0;
            });
            return true;
        });

    return exit_code;
}