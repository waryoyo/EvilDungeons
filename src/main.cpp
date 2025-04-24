// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;


static constexpr char const* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
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

static glm::mat4 createCubeMVP(const glm::vec3& position, const glm::vec3& scale,
    int width, int height) {
    glm::mat4 model = glm::rotate(
        glm::translate(
            glm::mat4(1.0f), position
        ) +
        glm::scale(glm::mat4(1.0f), scale),
        static_cast<float>(glfwGetTime()),
        glm::vec3(1.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective<float>
        (glm::radians(30.0f), static_cast<float>(width) / height, 0.2f, 100.0f);

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

    return projection * view * model;

}


int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SAMPLES, 4);
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);


    GLuint program = makeProgram();

    GLint mvpLoc = glGetUniformLocation(program, "uMVP");
    GLint colorLoc = glGetUniformLocation(program, "uColor");

    static constexpr float vertices[] = {
       -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,
       -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f
    };
    static constexpr unsigned int indices[] = {
        0,1,2, 2,3,0, 
        4,5,6, 6,7,4,
        0,1,5, 5,4,0,
        2,3,7, 7,6,2,
        0,3,7, 7,4,0,
        1,2,6, 6,5,1 
    };

    GLuint VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
 

    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(VAO, EBO);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);


    while (!glfwWindowShouldClose(window)) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<glm::mat4> mvps;
        mvps.push_back(createCubeMVP({ 0.0f, 0.0f, 0.0f }, { 0.5f ,0.5f, 0.5f }, w, h));
        mvps.push_back(createCubeMVP({ 2.7f, -1.5f, -0.4f }, { 0.5f ,0.5f, 0.5f }, w, h));

   
        for (const auto& MVP : mvps) {

            glUseProgram(program);
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr<float>(MVP));
            glUniform4f(colorLoc, 0.1f, 0.1f, 0.1f, 1.0f);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);


        }
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
