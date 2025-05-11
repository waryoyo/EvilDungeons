#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <engine/graphics/model.hpp>
#include <engine/graphics/mesh.hpp>
#include <engine/utils/types.hpp>

class Scene;
class Component;
class CameraComponent;

class GameObject {
public:
	GameObject(const std::string& n, const Scene* owner)
		: name(n), active(true), owner(owner)
	{};

	std::string getName() const { return name; }
	bool isActive() const { return active; }

	void setActive(bool a);
	void addComponent(std::unique_ptr<Component> comp);

	template<typename T>
	T* getComponent() const {
		for (auto& c : components)
			if (auto ptr = dynamic_cast<T*>(c.get()))
				return ptr;
		return nullptr;
	}


	void update(float deltaTime);
	void render(const RenderContext& context);

	void setActiveCamera(const CameraComponent* camera);
	CameraComponent* getActiveCamera();

	const Scene* getScene() const;


private:
	std::string name;
	const Scene* owner;
	bool active;
	std::vector<std::unique_ptr<Component>> components;
};