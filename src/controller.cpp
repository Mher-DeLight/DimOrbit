#include "../include/DimOrbit/controller.h"

namespace DimOrbit {

int Controller::getKey(const std::string& action) const {
    auto it = keyBinds.find(action);
    if (it != keyBinds.end())
        return it->second;
    return -1;
}

bool Controller::isActionPressed(const std::string& action) const {
    int key = getKey(action);
    if (key != -1)
        return DimEngineZ::input::isKeyPressed(key);
    return false;
}
void Controller::bindKey(const std::string& action, int key) {
    keyBinds[action] = key;
}
void Controller::unbindKey(const std::string& action) {
    keyBinds.erase(action);
}

void Controller::bindAxis(const std::string& action, int positiveKey, int negativeKey) {
    keyBinds[action + "_positive"] = positiveKey;
    keyBinds[action + "_negative"] = negativeKey;
}
void Controller::unbindAxis(const std::string& action) {
    keyBinds.erase(action + "_positive");
    keyBinds.erase(action + "_negative");
}
float Controller::getAxis(const std::string& axis) const {
    int positiveKey = getKey(axis + "_positive");
    int negativeKey = getKey(axis + "_negative");
    float value = 0.0f;
    return dez::input::getAxis(negativeKey, positiveKey);
}

void Controller::bindVec2(const std::string& action, int upKey, int downKey, int leftKey,
                          int rightKey) {
    keyBinds[action + "_up"] = upKey;
    keyBinds[action + "_down"] = downKey;
    keyBinds[action + "_left"] = leftKey;
    keyBinds[action + "_right"] = rightKey;
}
void Controller::unbindVec2(const std::string& action) {
    keyBinds.erase(action + "_up");
    keyBinds.erase(action + "_down");
    keyBinds.erase(action + "_left");
    keyBinds.erase(action + "_right");
}
DimEngineZ::Vec2 Controller::getVec2(const std::string& action) const {
    int upKey = getKey(action + "_up");
    int downKey = getKey(action + "_down");
    int leftKey = getKey(action + "_left");
    int rightKey = getKey(action + "_right");
    return DimEngineZ::Vec2(dez::input::getAxis(leftKey, rightKey),
                            dez::input::getAxis(downKey, upKey));
}

void Controller::bindVec3(const std::string& action, int upKey, int downKey, int leftKey,
                          int rightKey, int forwardKey, int backwardKey) {
    keyBinds[action + "_up"] = upKey;
    keyBinds[action + "_down"] = downKey;
    keyBinds[action + "_left"] = leftKey;
    keyBinds[action + "_right"] = rightKey;
    keyBinds[action + "_forward"] = forwardKey;
    keyBinds[action + "_backward"] = backwardKey;
}
void Controller::unbindVec3(const std::string& action) {
    keyBinds.erase(action + "_up");
    keyBinds.erase(action + "_down");
    keyBinds.erase(action + "_left");
    keyBinds.erase(action + "_right");
    keyBinds.erase(action + "_forward");
    keyBinds.erase(action + "_backward");
}
DimEngineZ::Vec3 Controller::getVec3(const std::string& action) const {
    int upKey = getKey(action + "_up");
    int downKey = getKey(action + "_down");
    int leftKey = getKey(action + "_left");
    int rightKey = getKey(action + "_right");
    int forwardKey = getKey(action + "_forward");
    int backwardKey = getKey(action + "_backward");
    return DimEngineZ::Vec3(dez::input::getAxis(leftKey, rightKey),
                            dez::input::getAxis(downKey, upKey),
                            dez::input::getAxis(backwardKey, forwardKey));
}

} // namespace DimOrbit
