#include <game/scenes/minecraftScene.hpp>
#include <game/world/worldComponent.hpp>
#include <game/world/chunk.hpp>


struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

MinecraftScene::MinecraftScene(GLFWwindow* window)
    : Scene(window)
{
    world.generate();
    lightSystem = std::make_unique<LightSystem>();
    renderSystem = std::make_unique<RenderSystem>();
    input = std::make_unique<InputManager>();

    auto camGO = std::make_unique<GameObject>("MainCamera", this);
    camGO->addComponent(std::make_unique<CameraComponent>(camGO.get(), window, input.get()));
    camGO->getComponent<CameraComponent>()->setPosition({ 0.0f, 20.0f, 3.0f });
    objects.push_back(std::move(camGO));
    world.ensureChunksNear({ 0.0f, 20.0f, 3.0f });

   /* auto worldGO = std::make_unique<GameObject>("World", this);
    worldGO->addComponent(std::make_unique<WorldComponent>(worldGO.get()));
    objects.push_back(std::move(worldGO));*/
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

bool intersectAABB(const glm::vec3& amin, const glm::vec3& amax,
    const glm::vec3& bmin, const glm::vec3& bmax)
{
    return (amin.x <= bmax.x && amax.x >= bmin.x) &&
        (amin.y <= bmax.y && amax.y >= bmin.y) &&
        (amin.z <= bmax.z && amax.z >= bmin.z);
}

void MinecraftScene::update(float dt)
{
    input->update(window);
    float speed = 5.0f * dt;

    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    glm::vec3 pos = camera->getPosition();
    glm::vec3 front = camera->getFront();
    glm::vec3 up = camera->getUp();
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, front));

    glm::vec3 cameraDelta = glm::vec3(0.0f);
    if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
        speed *= 3.0f;
    }
    if (input->isKeyDown(GLFW_KEY_SPACE)) {
        cameraDelta += speed * up;
    }
    if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        cameraDelta -= speed * up;
    }
    if (input->isKeyDown(GLFW_KEY_W)) {
        cameraDelta += speed * front;
    }
    if (input->isKeyDown(GLFW_KEY_S)) {
        cameraDelta -= speed * front;
    }
    if (input->isKeyDown(GLFW_KEY_A)) {
        cameraDelta += speed * cameraRight;
    }
    if (input->isKeyDown(GLFW_KEY_D)) {
        cameraDelta -= speed * cameraRight;
    }

    glm::vec3 newPos = pos + cameraDelta;
    world.ensureChunksNear(newPos);

    const glm::vec3 cameraHalfExtents(0.3f, 0.9f, 0.3f);
    auto isColliding = [&](const glm::vec3& testPos) -> bool {

        glm::ivec3 minBlock = glm::floor(testPos - cameraHalfExtents);
        glm::ivec3 maxBlock = glm::floor(testPos + cameraHalfExtents);

        for (int x = minBlock.x; x <= maxBlock.x; x++) {
            for (int y = minBlock.y; y <= maxBlock.y; y++) {
                for (int z = minBlock.z; z <= maxBlock.z; z++) {
                    if (world.getBlock(x, y, z) != BlockType::Air) {
                        // Block is solid, check AABB overlap
                        glm::vec3 blockMin(x, y, z);
                        glm::vec3 blockMax = blockMin + glm::vec3(1.0f);
                        glm::vec3 camMin = testPos - cameraHalfExtents;
                        glm::vec3 camMax = testPos + cameraHalfExtents;

                        bool overlap = (camMin.x < blockMax.x && camMax.x > blockMin.x) &&
                                       (camMin.y < blockMax.y && camMax.y > blockMin.y) &&
                                       (camMin.z < blockMax.z && camMax.z > blockMin.z);
                                       
                        if (overlap) return true;
                    }
                }
            }
        }
        return false;
    };

    glm::vec3 tryPos = pos;
    glm::vec3 attempt = newPos;

    glm::vec3 testX = tryPos; testX.x = attempt.x;
    if (!isColliding(testX)) tryPos.x = attempt.x;

    glm::vec3 testY = tryPos; testY.y = attempt.y;
    if (!isColliding(testY)) tryPos.y = attempt.y;

    glm::vec3 testZ = tryPos; testZ.z = attempt.z;
    if (!isColliding(testZ)) tryPos.z = attempt.z;

    camera->setPosition(tryPos);
    world.ensureChunksNear(tryPos);

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

    world.render(context);

    for (const auto& object : objects)
        object->render(context);
}

LightSystem* MinecraftScene::getLightSystem() const
{
    return lightSystem.get();
}
