// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;


struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
};

static Camera cameraObject = Camera();

static int screenWidth = 1280;
static int screenHeight = 720;

static float lastMouseX = 1280/2;
static float lastMouseY = 720/2;
static bool isPaused = false;
static bool showSettings = false;

static glm::mat4 createCubeModel(const glm::vec3& position, const glm::vec3& scale) {
    /*glm::mat4 model = glm::rotate(
        glm::translate(glm::mat4(1.0f), position),
        static_cast<float>(glfwGetTime()),
        glm::vec3(1.0f, 1.0f, 0.0f));*/

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);

    return model;
}


static glm::mat4 createCubeMVP(const glm::mat4 model,
    int width, int height) {

    glm::mat4 projection = glm::perspective<float>
        (glm::radians(45.0f), static_cast<float>(width) / height, 0.2f, 1000.0f);

    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
    glm::mat4 view = glm::lookAt(cameraObject.getCameraPos(), cameraObject.getCameraPos() + cameraObject.getCameraFront(), cameraObject.getCameraUp());

    return projection * view * model;
}

static void createCubeVAO(GLuint& VAO, GLuint& VBO, GLuint& EBO) {
    static constexpr Vertex vertices[] = {
        // front face
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

        // back face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

        // bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

        // top face
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

        // right face
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

        // left face
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    };

    static constexpr unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };


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

    glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(VAO, 2, 0);
    glEnableVertexArrayAttrib(VAO, 2);

    glVertexArrayElementBuffer(VAO, EBO);
}

static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (isPaused) {
        return;
    }
    cameraObject.handleMouse(window, xpos, ypos);

}

static void processInput(GLFWwindow* window, float deltaTime) {
    static bool escapePressedLastFrame = false;

    bool escapePressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (escapePressed && !escapePressedLastFrame) {
        if (!isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isPaused = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isPaused = false;
            cameraObject.setFirstMouse(true);
        }
    }

    escapePressedLastFrame = escapePressed;

    if (!isPaused) {
        cameraObject.handleKeyboard(window, deltaTime);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Texture texture = Texture("H.png");
    //Shader shader = Shader("super_cube/basic.vert", "super_cube/basic.frag");
    Shader lightShader = Shader("light/light.vert", "light/light.frag");


    Shader shader = Shader("model/basic.vert", "model/basic.frag");
    Model sonic = Model("sonic_the_hedgehog/scene.gltf");

    GLuint VAO, VBO, EBO;
    
    createCubeVAO(VAO, VBO, EBO);

    glm::vec3 lightPos = { 0.0f, 3.0f, -10.0f };
    glm::vec3 lightColor = { 0.8f, 0.8f, 0.75f };

    glm::vec3 materialAmbient = glm::vec3(0.2f);
    glm::vec3 materialDiffuse = glm::vec3(1.0f);
    glm::vec3 materialSpecular = glm::vec3(0.5f);
    GLfloat shininess = 32.0f;

    glm::vec3 lightAmbient = glm::vec3(0.7f);
    glm::vec3 lightDiffuse = glm::vec3(0.8f);
    glm::vec3 lightSpecular = glm::vec3(0.4f);

    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int w, h;
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        shader.use();

        glClearColor(0.2f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window, deltaTime);

        std::vector<glm::mat4> mvps;

        std::vector<glm::mat4> models;

        models.push_back(createCubeModel({ 0.0f, 0.0f, -5.0f }, { 1.5f ,1.5f, 1.5f }));

        texture.bind(0);
        shader.setInt("uTex", 0);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("cameraPos", cameraObject.getCameraPos());

        shader.setVec3("material.ambient", materialAmbient);
        shader.setVec3("material.diffuse", materialDiffuse);
        shader.setVec3("material.specular", materialSpecular);
        shader.setFloat("shininess", shininess);

        shader.setVec3("light.ambient", lightAmbient);
        shader.setVec3("light.diffuse", lightDiffuse);
        shader.setVec3("light.specular", lightSpecular);
        
        auto model = createCubeModel({ 0.0f, 0.0f, -5.0f }, { 1.5f ,1.5f, 1.5f });

        shader.setMat4("uModel", model);
        shader.setMat4("uMVP", createCubeMVP(model, w, h));

        sonic.draw(shader);

        //for (const auto& model : models) {
         

        //    glBindVertexArray(VAO);

            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        //}

        glm::mat4 lightModel = createCubeModel(lightPos, { 1.0f ,1.0f, 1.0f });
        glm::mat4 lightMVP = createCubeMVP(lightModel, w, h);
        lightShader.use();
        lightShader.setMat4("uMVP", lightMVP);
        lightShader.setVec3("uObjectColor", lightColor);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);


if (isPaused) {
    ImVec2 windowPos(0, 0); 
    ImVec2 windowSize(screenWidth * 0.9f, screenHeight * 0.9f);
    
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f)); 

    ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);  

    ImGui::Text("Game Paused");
    
    if (ImGui::Button("Resume", ImVec2(180, 50))) {  
        isPaused = false; 
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
        cameraObject.setFirstMouse(true); 
    }
    
    if (ImGui::Button("Light Settings", ImVec2(180, 50))) { 
        showSettings = !showSettings; 
    }

    if (ImGui::Button("Quit", ImVec2(180, 50))) {  
        glfwSetWindowShouldClose(window, GLFW_TRUE);  
    }
    
    ImGui::PopFont();
    
    ImGui::End();

    ImGui::PopStyleColor();


    if (showSettings) {
        ImVec2 settingsPos(0, screenHeight * 0.3f);  
        ImVec2 settingsSize(screenWidth * 0.9f, screenHeight * 0.6f); 

        ImGui::SetNextWindowPos(settingsPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(settingsSize, ImGuiCond_Always);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
        
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        
        ImGui::PushItemWidth(400); 
        ImGui::SliderFloat3("Light Position", &lightPos.x, -20.0f, 20.0f);
        ImGui::ColorEdit3("Light Color", &lightColor.x);
        ImGui::SliderFloat3("Material Ambient", &materialAmbient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Diffuse", &materialDiffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Material Specular", &materialSpecular.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Material Shininess", &shininess, 1.0f, 128.0f);
        ImGui::SliderFloat3("Light Ambient", &lightAmbient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Diffuse", &lightDiffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Light Specular", &lightSpecular.x, 0.0f, 1.0f);
        ImGui::PopItemWidth(); 
        
        ImGui::End();
        
        ImGui::PopStyleColor();
    }
}


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
