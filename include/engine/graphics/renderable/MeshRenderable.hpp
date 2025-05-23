#pragma once
#include "IRenderable.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/graphics/texture.hpp>

class MeshRenderable : public IRenderable {
public:
    MeshRenderable(GLuint vao, GLsizei count)
        : VAO(vao), indexCount(count) {}

    void draw(const Shader* shader) const override {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
private:
    GLuint VAO;
    GLsizei indexCount;
};
