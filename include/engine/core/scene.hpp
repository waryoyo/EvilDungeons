#pragma once

#include <GLFW/glfw3.h>

class Scene {
public:
	Scene(GLFWwindow* window) : window(window){};
	virtual ~Scene() = default;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
protected:
	const GLFWwindow* window;
};