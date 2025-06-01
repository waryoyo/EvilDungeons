#include <engine/core/inputManager.hpp>

void InputManager::update(GLFWwindow* window) {
    previousKeyStates = currentKeyStates;
    previousMouseButtonStates = currentMouseButtonStates;

    for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; ++key) {
        currentKeyStates[key] = (glfwGetKey(window, key) == GLFW_PRESS);
    }

    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
        currentMouseButtonStates[button] = (glfwGetMouseButton(window, button) == GLFW_PRESS);
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseDeltaX = mouseX - lastMouseX;
    mouseDeltaY = mouseY - lastMouseY;
}

// Keyboard
bool InputManager::isKeyDown(int key) const {
    auto it = currentKeyStates.find(key);
    return it != currentKeyStates.end() && it->second;
}

bool InputManager::wasKeyPressed(int key) const {
    bool now = isKeyDown(key);
    bool before = previousKeyStates.find(key) != previousKeyStates.end() && previousKeyStates.at(key);
    return now && !before;
}

bool InputManager::wasKeyReleased(int key) const {
    bool now = isKeyDown(key);
    bool before = previousKeyStates.find(key) != previousKeyStates.end() && previousKeyStates.at(key);
    return !now && before;
}

// Mouse
bool InputManager::isMouseButtonDown(int button) const {
    auto it = currentMouseButtonStates.find(button);
    return it != currentMouseButtonStates.end() && it->second;
}

bool InputManager::wasMouseButtonPressed(int button) const {
    bool now = isMouseButtonDown(button);
    bool before = previousMouseButtonStates.find(button) != previousMouseButtonStates.end() && previousMouseButtonStates.at(button);
    return now && !before;
}

bool InputManager::wasMouseButtonReleased(int button) const {
    bool now = isMouseButtonDown(button);
    bool before = previousMouseButtonStates.find(button) != previousMouseButtonStates.end() && previousMouseButtonStates.at(button);
    return !now && before;
}
