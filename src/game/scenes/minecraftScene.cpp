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
    //camGO->getComponent<TransformComponent>()->setPosition({ 0.0f, -10.0f, 0.0f });
    objects.push_back(std::move(camGO));

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
    world.ensureChunksNear(pos);


    const glm::vec3 camMinLocal = { -0.3f, 0.0f,-0.3f };
    const glm::vec3 camMaxLocal = { 0.3f, 1.8f, 0.3f };
    AABB camBox{
      newPos + camMinLocal,
      newPos + camMaxLocal
    };

    bool hit = false;
    for (auto& [coord, chunkPtr] : world.chunks) {
        glm::vec3 chunkBase = glm::vec3(coord * 32);
        for (int x = 0; x < 32 && !hit; ++x) {
            for (int y = 0; y < 32 && !hit; ++y) {
                for (int z = 0; z < 32; ++z) {
                    if (chunkPtr->getBlock({ x,y,z }) == BlockType::Air)
                        continue;

                    glm::vec3 bMin = chunkBase + glm::vec3(x, y, z);
                    glm::vec3 bMax = bMin + glm::vec3(1.0f);
                    if (intersectAABB(camBox.min, camBox.max, bMin, bMax)) {
                        hit = true;
                        break;
                    }
                }
            }
        }
        if (hit) break;
    }
    camera->setPosition(newPos);

  /*  if (!hit) {
        camera->setPosition(newPos);
        world.ensureChunksNear(newPos);
    }*/
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
