#pragma once

#include <memory>
#include <vector>
#include <engine/core/scene.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class SceneManager {
public:
	SceneManager(GLFWwindow* window) : window(window) {};

	void push(std::unique_ptr<Scene> scene);
	void pop();

	void update(float dt);
	void render();

	bool isEmpty();
private:
	const GLFWwindow* window;
	std::vector<std::unique_ptr<Scene>> sceneStack;
};