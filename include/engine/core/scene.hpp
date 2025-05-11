#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/components/cameraComponent.hpp>
#include <engine/core/lightSystem.hpp>

class GameObject;

class Scene {
public:
	Scene(GLFWwindow* window) : window(window){};
	virtual ~Scene() = default;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;

	void setActiveCamera(CameraComponent* cam) { activeCamera = cam; }
	CameraComponent* getActiveCamera() const { return activeCamera; }

	virtual LightSystem* getLightSystem() const { return nullptr; }

protected:
	GLFWwindow* window;
	CameraComponent* activeCamera = nullptr;
};