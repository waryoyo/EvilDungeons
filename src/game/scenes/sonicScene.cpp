#include <game/scenes/sonicScene.hpp>


SonicScene::SonicScene()
{

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Dark Dungeon", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!\n";
        glfwTerminate();
        return ;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    auto modelShader = std::make_unique<Shader>(
        "model/basic.vert", "model/basic.frag");
    auto lightShader = std::make_unique<Shader>(
        "light/light.vert", "light/light.frag");
    auto sonicModel = std::make_unique<Model>(
        "sonic_the_hedgehog/scene.gltf");

    auto camGO = std::make_unique<GameObject>("MainCamera");
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window));
    objects.push_back(camGO);

    auto sonicGO = std::make_unique<GameObject>("SonicModel");
    sonicGO->addComponent(std::make_unique<TransformComponent>(sonicGO.get()));
    sonicGO->addComponent(
        std::make_unique<RendererComponent>(
            sonicGO.get(), *sonicModel, *modelShader));
    sonicGO->getComponent<TransformComponent>()->setPosition({ 0.0f,0.0f,-5.0f });
    sonicGO->getComponent<TransformComponent>()->setScale(glm::vec3(1.0f));
    objects.push_back(std::move(sonicGO));

    auto lightGO = std::make_unique<GameObject>("LightCube");
    lightGO->addComponent(std::make_unique<TransformComponent>(lightGO.get()));
    lightGO->addComponent(
        std::make_unique<RendererComponent>(
            lightGO.get(), World::getCubeModel(), *lightShader));
}

SonicScene::~SonicScene()
{
}

void SonicScene::onEnter()
{
}

void SonicScene::onExit()
{
}

void SonicScene::update(float deltaTime)
{
}

void SonicScene::render()
{
}
