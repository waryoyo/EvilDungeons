// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dark Dungeon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
