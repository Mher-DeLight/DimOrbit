# DimOrbit
_DimOrbit_ is a Newtonian spacecraft/orbit simulator based on my Raylib wrapper/game engine [_DimEngineZ_](https://www.github.com/Mher-DeLight/DimEngineZ). It is meant to be simple and easy-to-use.

> [!WARNING]
> Some changes are made to _DimEngineZ_ directly from _DimOrbit_ without modifying the original copy. This may include bug fixes that may not be mirrored immediately to _DimEngineZ_.

## Example
Here's an example of the sun, an XZ Clue plane, and a spacecraft called "Explorer" that orbits around it.
```cpp
#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    using Vec3 = dez::Vec3;

    dez::manager::init(1000, 800, "Newtonian Gravity Simulation"); // window width, window height, window title

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
        .mass = 1e-3,
    });
    explorer->body->beginOrbit(*sun.get(), 5.0f, 0.0f);
    renderer.addAttribute(*explorer->body.get(), dor::RENATR_SHOW_NAME);

    constexpr float CAM_SPEED = 15.0f;
    constexpr float CAM_LOOK_SPEED = 2.0f;

    return dez::manager::fixedloop(
        60, 60,
        [&](float delta) {
            camera.moveRight(dez::input::getAxis(KEY_A, KEY_D) * CAM_SPEED * delta);
            camera.moveForward(dez::input::getAxis(KEY_S, KEY_W) * CAM_SPEED * delta);

            camera.lookAround(-dez::input::getAxis(KEY_LEFT, KEY_RIGHT) * CAM_LOOK_SPEED * delta,
                              dez::input::getAxis(KEY_DOWN, KEY_UP) * CAM_LOOK_SPEED * delta);

            explorer->tick(delta);
            system.tick(delta);
            return true;
        },
        [&]() {
            renderer.doindraw(BLACK, [&]() {
                renderer.doin3d(camera, [&]() {
                    renderer.render(system);
                    renderer.displayVector(explorer->physics->core.velocity,
                                           explorer->physics->transform.position);
                    renderer.displayVector(explorer->engine.thrust * 1e3f,
                                           explorer->physics->transform.position, RED);
                    return 0;
                });

                renderer.renderLabels(system, RED);
                return 0;
            });

            return true;
        });
}
```
As you can see, there is a strong separation between the actual physics (`dor::CelestialSystem`) and the rendering of the objects (`dor::Renderer`). Things such as labels, vectors, and drawing in 
general are handled exclusively by the Renderer.

> [!NOTE]
> Due to current architecture complications, `dor::BasicSpacecraft` objects have to be `tick()`-ed manually. I am currently working on making the owning `dor::CelestialSystem` handle that.
