#pragma once

#include "component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TransformComponent : public Component {
public:
    TransformComponent(GameObject* owner)
        : Component(owner),
        position(0.0f,0.0f,-5.0f), rotation(0.0f), scale(1.0f),
        modelMatrix(1.0f)
    {}

    void setPosition(const glm::vec3& p) { position = p; }
    void setRotation(const glm::vec3& r) { rotation = r; }
    void setScale(const glm::vec3& s) { scale = s; }

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }

    void update(float deltaTime) {
        calculateModelMatrix(deltaTime);
    }

    const glm::mat4& getModelMatrix() {
        return modelMatrix;
    }
private:
    glm::vec3 position, rotation, scale;
    glm::mat4 modelMatrix;

    void calculateModelMatrix(float deltaTime) {
        modelMatrix = glm::rotate(
            glm::translate(glm::mat4(1.0f), position),
            deltaTime,
            glm::vec3(1.0f, 1.0f, 0.0f));

        modelMatrix = glm::scale(modelMatrix, scale);
    }
};