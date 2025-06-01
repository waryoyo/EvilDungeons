#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
    void update(GLFWwindow* window);

    // Keyboard
    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool wasKeyReleased(int key) const;

    // Mouse buttons
    bool isMouseButtonDown(int button) const;
    bool wasMouseButtonPressed(int button) const;
    bool wasMouseButtonReleased(int button) const;

    // Mouse movement
    double getMouseX() const { return mouseX; }
    double getMouseY() const { return mouseY; }
    double getMouseDeltaX() const { return mouseDeltaX; }
    double getMouseDeltaY() const { return mouseDeltaY; }

private:
    std::unordered_map<int, bool> currentKeyStates;
    std::unordered_map<int, bool> previousKeyStates;

    std::unordered_map<int, bool> currentMouseButtonStates;
    std::unordered_map<int, bool> previousMouseButtonStates;

    double mouseX = 0.0, mouseY = 0.0;
    double lastMouseX = 0.0, lastMouseY = 0.0;
    double mouseDeltaX = 0.0, mouseDeltaY = 0.0;
};
