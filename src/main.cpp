// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;


struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

static int screenWidth = 1280;
static int screenHeight = 720;

static float lastMouseX = 1280/2;
static float lastMouseY = 720/2;
static bool firstMouse = true;
static float yaw = -90.0f, pitch = 0.0f;
static const float sensitivity = 0.1f;

static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

static glm::mat4 createCubeMVP(const glm::vec3& position, const glm::vec3& scale,
    int width, int height) {

    //glm::mat4 model = glm::rotate(
    //    glm::translate(glm::mat4(1.0f), position),
    //    static_cast<float>(glfwGetTime()),
    //    glm::vec3(1.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);

    glm::mat4 projection = glm::perspective<float>
        (glm::radians(45.0f), static_cast<float>(width) / height, 0.2f, 1000.0f);

    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    return projection * view * model;
}



static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec3 direction;

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;

    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction.y = glm::sin(glm::radians(pitch));
    direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

static void processInput(GLFWwindow* window, float deltaTime) {
    float speed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        speed *= 4.0f;
    }

    const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos += speed * cameraRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos -= speed * cameraRight;
    }
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

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Dark Dungeon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    Texture texture = Texture("H.png");
    Shader shader = Shader("basic.vert", "basic.frag");
    Model sonic = Model("sonic_the_hedgehog/scene.gltf");
    Shader sonicShader = Shader("model/basic.vert", "model/basic.frag");

    static constexpr Vertex vertices[] = {
        {{-0.5f,-0.5f,-0.5f},{0.0f,0.0f}},
        {{ 0.5f,-0.5f,-0.5f},{1.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{1.0f,1.0f}},
        {{-0.5f, 0.5f,-0.5f},{0.0f,1.0f}},

        {{-0.5f,-0.5f, 0.5f},{1.0f,0.0f}},
        {{ 0.5f,-0.5f, 0.5f},{0.0f,0.0f}},
        {{ 0.5f, 0.5f, 0.5f},{0.0f,1.0f}},
        {{-0.5f, 0.5f, 0.5f},{1.0f,1.0f}},

        {{-0.5f,-0.5f,-0.5f},{0.0f,1.0f}},
        {{ 0.5f,-0.5f,-0.5f},{1.0f,1.0f}},
        {{ 0.5f,-0.5f, 0.5f},{1.0f,0.0f}},
        {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f}},

        {{-0.5f, 0.5f,-0.5f},{0.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{1.0f,0.0f}},
        {{ 0.5f, 0.5f, 0.5f},{1.0f,1.0f}},
        {{-0.5f, 0.5f, 0.5f},{0.0f,1.0f}},

        {{-0.5f,-0.5f,-0.5f},{1.0f,0.0f}},
        {{-0.5f, 0.5f,-0.5f},{1.0f,1.0f}},
        {{-0.5f, 0.5f, 0.5f},{0.0f,1.0f}},
        {{-0.5f,-0.5f, 0.5f},{0.0f,0.0f}},

        {{ 0.5f,-0.5f,-0.5f},{0.0f,0.0f}},
        {{ 0.5f, 0.5f,-0.5f},{0.0f,1.0f}},
        {{ 0.5f, 0.5f, 0.5f},{1.0f,1.0f}},
        {{ 0.5f,-0.5f, 0.5f},{1.0f,0.0f}},
    };

    static constexpr unsigned int indices[] = {
        0,1,2, 2,3,0, 
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20 
    };


    GLuint VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
 

    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glVertexArrayAttribBinding(VAO, 0, 0);
    glEnableVertexArrayAttrib(VAO, 0);


    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glVertexArrayAttribBinding(VAO, 1, 0);
    glEnableVertexArrayAttrib(VAO, 1);

    glVertexArrayElementBuffer(VAO, EBO);

    float lastFrameTime = 0.0f;


    while (!glfwWindowShouldClose(window)) {
        int w, h;
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        //shader.use();
        sonicShader.use();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window, deltaTime);

        std::vector<glm::mat4> mvps;
        mvps.push_back(createCubeMVP({ 0.0f, 0.0f, -5.0f }, { 1.5f ,1.5f, 1.5f }, w, h));
        mvps.push_back(createCubeMVP({ 1.0f, 4.0f, -2.0f }, { 0.5f ,0.5f, 0.5f }, w, h));

        //texture.bind(0);
        shader.setInt("uTex", 0);
        shader.setMat4("uMVP", createCubeMVP({ 0.0f, 0.0f, 0.0f }, { 1.0f ,1.0f, 1.0f }, w, h));

        sonic.draw();
       /* for (const auto& MVP : mvps) {

            shader.setMat4("uMVP", MVP);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }*/
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
