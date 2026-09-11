#include <DimOrbit/DimOrbit.h>

int main(int, char**) {
    namespace dez = DimEngineZ;
    dez::manager::init(1000, 800, "Welcome");

    return mainloop(60, {
        if (dez::input::isKeyPressed(KEY_Q)) {
            exit(0);
        }
    });
}