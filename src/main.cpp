// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;


static constexpr char const* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec2 aPos;
void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static constexpr char const* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
uniform vec4 uColor;
void main()
{
    FragColor = uColor;
}
)";


static GLuint makeShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    return shader;
}

static GLuint makeProgram() {
    GLuint vertexShader = makeShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = makeShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dark Dungeon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    GLuint program = makeProgram();
    GLint colorLoc = glGetUniformLocation(program, "uColor");

    static constexpr float vertices[] = {
      -0.5f,  0.5f,
      -0.5f, -0.5f,
       0.5f,  0.5f, 
       0.5f, -0.5f   
    };

    GLuint VAO, VBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
 

    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 2 * sizeof(float));

    glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform4f(colorLoc, 0.1f, 0.1f, 0.1f, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
