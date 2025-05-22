#include <game/scenes/minecraftScene.hpp>
#include <game/world/worldComponent.hpp>
#include <game/world/chunk.hpp>
#include <engine/graphics/texture.hpp>


unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;


float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

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
    glCreateVertexArrays(1, &skyboxVAO);
    glCreateBuffers(1, &skyboxVBO);

    glNamedBufferData(skyboxVBO,
        sizeof(skyboxVertices),
        skyboxVertices,
        GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(skyboxVAO, 0, skyboxVBO, 0, 3 * sizeof(float));

    glVertexArrayAttribFormat(skyboxVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(skyboxVAO, 0, 0);
    glEnableVertexArrayAttrib(skyboxVAO, 0);

    cubemapTexture = loadCubemap();


    if (!ShaderManager::Get("skyShader"))
        ShaderManager::Load("skyShader", "sky/basic.vert", "sky/basic.frag");
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
    renderSky();

    world.render(context);

    for (const auto& object : objects)
        object->render(context);

}

LightSystem* MinecraftScene::getLightSystem() const
{
    return lightSystem.get();
}

unsigned int MinecraftScene::loadCubemap()
{
    stbi_set_flip_vertically_on_load(0);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(1);
    return textureID;
}

void MinecraftScene::renderSky() {
    if (!ShaderManager::Get("skyShader")) {
        std::cerr << "skyShader not loaded" << '\n';
        return;
    }
    const auto shader = ShaderManager::Get("skyShader");

    const auto& camera = objects[0]->getComponent<CameraComponent>(); // ONLY DOING THIS HERE WILL NEVER DO THIS, this is just for testing
    const auto projection = camera->getProjection();
    const auto view = glm::mat4(glm::mat3(camera->getView()));
    glDepthMask(GL_FALSE);
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    
};
