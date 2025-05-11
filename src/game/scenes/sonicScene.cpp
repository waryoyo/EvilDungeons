#include <game/scenes/sonicScene.hpp>
#include <engine/core/gameObject.hpp>
#include <engine/graphics/binder/PhongBinder.hpp>


SonicScene::SonicScene(GLFWwindow* window)
    : Scene(window)
{

    lightSystem = std::make_unique<LightSystem>();
    renderSystem = std::make_unique<RenderSystem>();

    // TODO: some weird flickering is happening make sure to fix that.
    light = LightNew(glm::vec3(0.7f), glm::vec3(0.8f), glm::vec3(0.4f), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f);

    input = std::make_unique<InputManager>();
    ShaderManager::Load("modelShader", "model/basic.vert", "model/basic.frag");
    ShaderManager::Load("lightShader", "light/light.vert", "light/light.frag");


    auto camGO = std::make_unique<GameObject>("MainCamera", this);
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window, input.get()));
    objects.push_back(std::move(camGO));


    auto sonicGO = std::make_unique<GameObject>("SonicModel", this);
    sonicGO->addComponent(std::make_unique<TransformComponent>(sonicGO.get()));
    sonicGO->getComponent<TransformComponent>()->setPosition({ 0.0f,0.0f,-5.0f });
    sonicGO->getComponent<TransformComponent>()->setScale(glm::vec3(1.0f));
    renderSystem->addModel(sonicGO.get(), "sonic_the_hedgehog/scene.gltf", ShaderManager::Get("modelShader"));
    objects.push_back(std::move(sonicGO));

    auto lightGO = std::make_unique<GameObject>("LightCube", this);
    lightGO->addComponent(std::make_unique<TransformComponent>(lightGO.get()));
    lightGO->getComponent<TransformComponent>()->setPosition({ 15.0f,5.0f,-10.0f });
    lightGO->addComponent(std::make_unique<LightComponent>(lightGO.get(), light));
    renderSystem->addColorCube(lightGO.get(), { 1.0f, 1.0f, 1.0 }, ShaderManager::Get("lightShader"));
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
    input->update(window);
    float speed = 5.0f * dt;

    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    glm::vec3 pos = camera->getPosition();
    glm::vec3 front = camera->getFront();
    glm::vec3 up = camera->getUp();
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, front));


    if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 3.0f;
    }
    if (input->isKeyDown(GLFW_KEY_SPACE)) {
        pos += speed * up;
    }
    if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        pos -= speed * up;
    }
    if (input->isKeyDown(GLFW_KEY_W)) {
        pos += speed * front;
    }
    if (input->isKeyDown(GLFW_KEY_S)) {
        pos -= speed * front;
    }
    if (input->isKeyDown(GLFW_KEY_A)) {
        pos += speed * cameraRight;
    }
    if (input->isKeyDown(GLFW_KEY_D)) {
        pos -= speed * cameraRight;
    }

    camera->setPosition(pos);
    if (input->wasKeyPressed(GLFW_KEY_P) || input->wasKeyPressed(GLFW_KEY_ESCAPE)) {
        if (!isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isPaused = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isPaused = false;
            camera->setFirstMouse(true);
        }
    }
    camera->setIsActive(!isPaused);
    for (const auto& object : objects)
        object->update(dt);
}

void SonicScene::render()
{
    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    const auto VP = camera->getProjection() * camera->getView();
    std::vector vec = std::vector<LightNew>();
    vec.push_back(light);
    RenderContext context = RenderContext(CameraData(camera->getPosition(), VP), vec);

    for (const auto& object : objects)
        object->render(context);
}

LightSystem* SonicScene::getLightSystem() const
{
    return lightSystem.get();
}
