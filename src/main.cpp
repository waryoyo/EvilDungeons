// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

static int screenWidth = 1280;
static int screenHeight = 720;

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {

    std::cerr << "GL DEBUG: " << message
        << " | type=" << type
        << " | severity=" << severity
        << " | id=" << id << "\n";
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);


    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Dark Dungeon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, nullptr);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);


    auto sceneManager = SceneManager(window);
    sceneManager.push(std::make_unique<SonicScene>(window));
    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
       
        int w, h;
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.2f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneManager.update(deltaTime);
        sceneManager.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
        
      /*  inputManager.update(window);
        if (inputManager.wasKeyPressed(GLFW_KEY_P) or inputManager.wasKeyPressed(GLFW_KEY_ESCAPE)) {
            if (!isPaused) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                isPaused = true;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                isPaused = false;
                cameraObject.setFirstMouse(true);
            }
        }*/

    }

    glfwTerminate();
    return 0;
}
