// main.cpp : Defines the entry point for the application.
//

#include "main.h"
#include <chrono>

// Audio
ma_decoder decoder;
ma_device device;

auto lastTime = std::chrono::high_resolution_clock::now();
int frameCount = 0;
float fps = 0.0f;

using namespace std;

static int screenWidth = 1280;
static int screenHeight = 720;

// OpenGL debug callback
static void GLAPIENTRY MessageCallback(GLenum source,
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

// Audio callback
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder_read_pcm_frames((ma_decoder*)pDevice->pUserData, pOutput, frameCount, NULL);
    (void)pInput;
}

int main()
{
    // Initialize GLFW
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

    // OpenGL settings
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, nullptr);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Load scene
    auto sceneManager = SceneManager(window);
    sceneManager.push(std::make_unique<MinecraftScene>(window));

    float lastFrameTime = 0.0f;

    // ---------- AUDIO SETUP ----------
    if (ma_decoder_init_file("assets/music/minecraftsoundtrack.mp3", NULL, &decoder) != MA_SUCCESS) {
        std::cerr << "Failed to load audio file\n";
        return -1;
    }

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "Failed to open playback device\n";
        ma_decoder_uninit(&decoder);
        return -1;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Failed to start playback device\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -1;
    }

    // ---------- GAME LOOP ----------
    while (!glfwWindowShouldClose(window)) {

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();

        if (dt >= 1.0f) {
            fps = frameCount / dt;
            frameCount = 0;
            lastTime = currentTime;
        }

        std::string title = "Not Minecraft - FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window, title.c_str());

        int w, h;
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glfwPollEvents();

        glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneManager.update(deltaTime);
        sceneManager.render();

        glfwSwapBuffers(window);
    }

    // Clean up audio
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    glfwTerminate();
    return 0;
}

void renderSky() {
    // Empty placeholder
}
