#pragma once
#include "common.h"

namespace DimEngineZ::input {

int getAxis(int keyNeg = KEY_LEFT, int keyPos = KEY_RIGHT);
Vec2 getVector2(int keyNegX = KEY_LEFT, int keyPosX = KEY_RIGHT, int keyNegY = KEY_DOWN,
                int keyPosY = KEY_UP);
Vec3 getVector3(int keyNegX, int keyPosX, int keyNegY, int keyPosY, int keyNegZ, int keyPosZ);

bool isKeyPressed(int key);
bool isKeyDown(int key);
}; // namespace DimEngineZ::input