#include <game/scenes/sonicScene.hpp>


SonicScene::SonicScene(GLFWwindow* window)
    : Scene(window)
{
    //auto modelShader = std::make_unique<Shader>(
    //    "model/basic.vert", "model/basic.frag");

    // TODO: some weird flickering is happening make sure to fix that.

    input = std::make_unique<InputManager>();

    modelShader = std::make_unique<Shader>("model/basic.vert", "model/basic.frag");
    lightShader = std::make_unique<Shader>("light/light.vert", "light/light.frag");

    //auto lightShader = std::make_unique<Shader>(
    //    "light/light.vert", "light/light.frag");
    auto sonicModel = std::make_unique<Model>(
        "sonic_the_hedgehog/scene.gltf");

    auto sonicRenderable = std::make_unique<ModelRenderable>(std::move(sonicModel));

    material = Material(glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(0.5f), 32.0f);
    light = Light(glm::vec3(0.7f), glm::vec3(0.8f), glm::vec3(0.4f));

    auto camGO = std::make_unique<GameObject>("MainCamera");
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window, input.get()));
    objects.push_back(std::move(camGO));

    auto sonicGO = std::make_unique<GameObject>("SonicModel");
    sonicGO->addComponent(std::make_unique<TransformComponent>(sonicGO.get()));
    sonicGO->addComponent(
        std::make_unique<RendererComponent>(
            sonicGO.get(), std::move(sonicRenderable), modelShader.get(), material, light));
    sonicGO->getComponent<TransformComponent>()->setPosition({ 0.0f,0.0f,-5.0f });
    sonicGO->getComponent<TransformComponent>()->setScale(glm::vec3(1.0f));
    objects.push_back(std::move(sonicGO));

    //GLuint VAO, VBO, EBO;
    //MeshFactory::makeCube(VAO, VBO, EBO); // We need a better thing for this, interface shouldnt know what a VAO is even unless needed

    //auto meshRenderer = std::make_unique<MeshRenderable>(VAO, 36);
    //auto lightGO = std::make_unique<GameObject>("LightCube");
    //lightGO->addComponent(std::make_unique<TransformComponent>(lightGO.get()));
    //lightGO->addComponent(
    //    std::make_unique<RendererComponent>(
    //        lightGO.get(), std::move(meshRenderer), lightShader.get(), material, light));
    //// Make it optional to pass material and light
    //objects.push_back(std::move(lightGO));
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
        } else {
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
    for (const auto& object : objects)
        object->render(VP);
}
