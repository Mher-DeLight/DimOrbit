#pragma once
#include "common.h"
#include <map>

namespace DimOrbit {

struct Controller {
    std::map<std::string, int> keyBinds;

    int getKey(const std::string& action) const;

    void bindKey(const std::string& action, int key);
    void unbindKey(const std::string& action);
    bool isActionPressed(const std::string& action) const;

    void bindAxis(const std::string& action, int positiveKey, int negativeKey);
    void unbindAxis(const std::string& action);
    float getAxis(const std::string& axis) const;

    void bindVec2(const std::string& action, int upKey, int downKey, int leftKey, int rightKey);
    void unbindVec2(const std::string& action);
    DimEngineZ::Vec2 getVec2(const std::string& action) const;

    void bindVec3(const std::string& action, int upKey, int downKey, int leftKey, int rightKey,
                  int forwardKey, int backwardKey);
    void unbindVec3(const std::string& action);
    DimEngineZ::Vec3 getVec3(const std::string& action) const;
};

} // namespace DimOrbit