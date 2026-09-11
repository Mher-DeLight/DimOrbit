#include <DimOrbit/DimOrbit.h>
#include <memory>

int main(int, char**) {
    namespace dez = DimEngineZ;
    namespace dor = DimOrbit;
    dez::manager::init(1000, 800, "Welcome");

    auto camera = dez::Camera({0.0f, 5.0f, 10.0f},
                              dez::CameraOptions{
                                  .fovy = 90.0f,
                              },
                              MAIN_CAMERA);
    camera.setTarget(dez::Vec3{0.0f, 0.0f, 0.0f});

    auto mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    auto drawobj = dez::DrawObject(std::move(mesh), dez::Transform(), RED);
    auto physicsObject = std::make_unique<dez::PhysicsObject>(std::move(drawobj));
    auto gravityObj = dor::GravityBody(std::move(physicsObject));
    gravityObj.physics->core.mass = 1.0e11f;

    mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    drawobj = dez::DrawObject(std::move(mesh), dez::Transform(), GREEN);
    physicsObject = std::make_unique<dez::PhysicsObject>(std::move(drawobj));
    auto gravityObj2 = dor::GravityBody(std::move(physicsObject));
    gravityObj2.physics->transform.goTo(dez::Vec3{3.0f, 0.0f, 0.0f});
    gravityObj2.physics->core.mass = 1.0e11f;

    return dez::manager::main(60, [&](float delta) {
        if (dez::input::isKeyPressed(KEY_Q)) {
            exit(0);
        }

        dor::gravity::tick(delta);
        dez::manager::tick(delta);
        return true;
    });
}