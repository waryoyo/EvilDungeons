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
        GLenum err = glGetError();
     /*   if (err != GL_NO_ERROR) {
            std::cerr
                << "[OpenGL Error] "
                << " (0x" << std::hex << err << std::dec << ")\n";
        }*/
    }
private:
    GLuint VAO;
    GLsizei indexCount;
};
