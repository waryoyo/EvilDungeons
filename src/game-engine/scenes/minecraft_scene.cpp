#include <game_engine/scenes/minecraftScene.hpp>
#include <game_engine/world/worldComponent.hpp>
#include <game_engine/world/chunk.hpp>


MinecraftScene::MinecraftScene(GLFWwindow* window)
    : Scene(window)
{
    lightSystem = std::make_unique<LightSystem>();
    renderSystem = std::make_unique<RenderSystem>();
    input = std::make_unique<InputManager>();

    auto playerGO = std::make_unique<GameObject>("MainPlayer", this);
    playerGO->addComponent(std::make_unique<CameraComponent>(playerGO.get(), window, input.get()));
    playerGO->addComponent(std::make_unique<PlayerComponent>(playerGO.get(), window, input.get()));
    objects.push_back(std::move(playerGO));

   /* auto camGO = std::make_unique<GameObject>("MainCamera", this);
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window, input.get()));
    objects.push_back(std::move(camGO));*/

    auto worldGO = std::make_unique<GameObject>("World", this);
    worldGO->addComponent(std::make_unique<WorldComponent>(worldGO.get()));
    objects.push_back(std::move(worldGO));

}

MinecraftScene::~MinecraftScene()
{
}

void MinecraftScene::onEnter()
{
}

void MinecraftScene::onExit()
{
}

void MinecraftScene::update(float dt)
{
    input->update(window);
    float speed = 5.0f * dt;

    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing

    // glm::vec3 pos = camera->getPosition();
    // glm::vec3 front = camera->getFront();
    // glm::vec3 up = camera->getUp();
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, front));


    // if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
    //     speed *= 3.0f;
    // }
    // if (input->isKeyDown(GLFW_KEY_SPACE)) {
    //     pos += speed * up;
    // }
    // if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
    //     pos -= speed * up;
    // }
    // if (input->isKeyDown(GLFW_KEY_W)) {
    //     pos += speed * front;
    // }
    // if (input->isKeyDown(GLFW_KEY_S)) {
    //     pos -= speed * front;
    // }
    // if (input->isKeyDown(GLFW_KEY_A)) {
    //     pos += speed * cameraRight;
    // }
    // if (input->isKeyDown(GLFW_KEY_D)) {
    //     pos -= speed * cameraRight;
    // }

    // camera->setPosition(pos);
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

void MinecraftScene::render()
{
    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    const auto VP = camera->getProjection() * camera->getView();
    RenderContext context = RenderContext(CameraData(camera->getPosition(), VP), {});

    for (const auto& object : objects)
        object->render(context);
}

LightSystem* MinecraftScene::getLightSystem() const
{
    return lightSystem.get();
}

