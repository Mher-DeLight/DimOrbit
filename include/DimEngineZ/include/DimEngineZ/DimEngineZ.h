#pragma once

// INCLUDES
#include "common.h"
#include "input.h"
#include "logger.h"
#include "manager.h"
#include "physics.h"

// MACROS
#define MAIN_CAMERA true
#define NOT_MAIN_CAMERA false
#define exitwcode(code)                                                                            \
    DimEngineZ::manager::exitCode = code;                                                          \
    return false;
#define mainloop(framerate, body)                                                                  \
    DimEngineZ::manager::main(framerate, [&](float delta) { body return true; });