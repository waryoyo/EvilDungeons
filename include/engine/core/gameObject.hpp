#pragma once

#include <string>

#include <glm/glm.hpp>

#include <engine/graphics/model.hpp>
#include <engine/graphics/mesh.hpp>
#include <engine/utils/types.hpp>



class GameObject {
public:
	GameObject(const std::string& name, const Model& meshes);
	GameObject(const std::string& name, const Mesh* meshes);

	void setPosition(const glm::vec3& position);
	void setRotation(const glm::vec3& rotation);
	void setScale(const glm::vec3& scale);
	void setMaterial(const Material& material);
	void setActive(bool active);

	glm::vec3 getPosition() const { return position; }
	glm::vec3 setRotation() const { return rotation; }
	glm::vec3 getScale() const { return scale; }
	Material getMaterial() const { return material; }
	const glm::mat4& getModelMatrix() const { return modelMatrix; }
	std::string getName() const { return name; }
	bool isActive() const { return active; }

	virtual void update(float deltaTime);
	virtual void render(const glm::mat4& viewProjectionMatrix);


private:
	std::string name;

	Shader shader;
	Material material;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 modelMatrix;

	bool active;

	void calculateModelMatrix();

};