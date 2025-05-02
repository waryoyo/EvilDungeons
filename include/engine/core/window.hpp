#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Window {
public:
	Window(const std::string& title, int width, int height);
	~Window();

	void pollEvents();
	void swapBuffers();
	bool shouldClose() const;
	glm::uvec2 getSize() const;

	GLFWwindow* GetGLFWWindow() const;

private:
	GLFWwindow* window;
};