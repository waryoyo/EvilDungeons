#include <engine_core/core/inputManager.hpp>

void InputManager::update(GLFWwindow *window){
    previousKeyStates = currentKeyStates;

    for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; ++key) {
        currentKeyStates[key] = (glfwGetKey(window, key) == GLFW_PRESS);
    }
}

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
