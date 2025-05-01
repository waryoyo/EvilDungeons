#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager{
public:
    void update(GLFWwindow* window);
    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool wasKeyReleased(int key) const;

private:
    std::unordered_map<int, bool> currentKeyStates;
    std::unordered_map<int, bool> previousKeyStates;
};