#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <engine/graphics/model.hpp>
#include <engine/graphics/mesh.hpp>
#include <engine/utils/types.hpp>
#include <engine/components/component.hpp>


class GameObject {
public:
	GameObject(const std::string& n)
		: name(n) {};

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
	void render(const glm::mat4& VP);


private:
	std::string name;
	bool active;
	std::vector<std::unique_ptr<Component>> components;
};