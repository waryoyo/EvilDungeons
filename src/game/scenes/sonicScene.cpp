#include <game/scenes/sonicScene.hpp>


SonicScene::SonicScene(GLFWwindow* window) 
    : Scene(window)
{
    //auto modelShader = std::make_unique<Shader>(
    //    "model/basic.vert", "model/basic.frag");

    modelShader = std::make_unique<Shader>("model/basic.vert", "model/basic.frag");
    lightShader = std::make_unique<Shader>("light/light.vert", "light/light.frag");

    //auto lightShader = std::make_unique<Shader>(
    //    "light/light.vert", "light/light.frag");
    auto sonicModel = std::make_unique<Model>(
        "sonic_the_hedgehog/scene.gltf");

    auto sonicRenderable = std::make_unique<ModelRenderable>(std::move(sonicModel));

    auto material = Material(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(0.5f));
    auto light = Light(glm::vec3(0.7f), glm::vec3(0.8f), glm::vec3(0.4f));

    auto camGO = std::make_unique<GameObject>("MainCamera");
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window));
    objects.push_back(std::move(camGO));

    auto sonicGO = std::make_unique<GameObject>("SonicModel");
    sonicGO->addComponent(std::make_unique<TransformComponent>(sonicGO.get()));
    sonicGO->addComponent(
        std::make_unique<RendererComponent>(
            sonicGO.get(), std::move(sonicRenderable), modelShader.get(), material, light));
    sonicGO->getComponent<TransformComponent>()->setPosition({ 0.0f,0.0f,-5.0f });
    sonicGO->getComponent<TransformComponent>()->setScale(glm::vec3(1.0f));
    objects.push_back(std::move(sonicGO));

    GLuint VAO, VBO, EBO;
    MeshFactory::makeCube(VAO, VBO, EBO); // We need a better thing for this, interface shouldnt know what a VAO is even unless needed

    auto meshRenderer = std::make_unique<MeshRenderable>(VAO, 36);
    auto lightGO = std::make_unique<GameObject>("LightCube");
    lightGO->addComponent(std::make_unique<TransformComponent>(lightGO.get()));
    lightGO->addComponent(
        std::make_unique<RendererComponent>(
            lightGO.get(), std::move(meshRenderer), lightShader.get(), material, light));
    // Make it optional to pass material and light
    objects.push_back(std::move(lightGO));
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

void SonicScene::update(float dt)
{
    for (const auto& object : objects)
        object->update(dt);
}

void SonicScene::render()
{
    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    const auto VP = camera->getProjection() * camera->getView();
    std::cout << VP[0][0] << '\n';
    for (const auto& object : objects)
        object->render(VP);
}
