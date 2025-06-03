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
    camGO->getComponent<CameraComponent>()->setPosition({ 0.0f, 70.0f, 3.0f });
    objects.push_back(std::move(camGO));
    world.ensureChunksNear({ 0.0f, 20.0f, 3.0f });

    if (!ShaderManager::Get("crosshair"))
        ShaderManager::Load("crosshair", "crosshair/crosshair.vert", "crosshair/crosshair.frag");
    Shader* crosshairShader = ShaderManager::Get("crosshair"); // or however you manage shaders
    crosshairRenderer = new CrosshairRenderer("assets/textures/crosshair.png", crosshairShader);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW/OpenGL (do this after creating your window)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImFont* font = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("assets/fonts/Minecraftia-Regular.ttf", 36.0f);
    io.Fonts->Build();

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
    availableBlocks = {
        BlockType::Dirt,
        BlockType::Grass,
        BlockType::Stone,
        BlockType::Sand
    };
    selectedBlockIndex = 0;
    selectedBlock = availableBlocks[selectedBlockIndex];



    if (!ShaderManager::Get("skyShader"))
        ShaderManager::Load("skyShader", "sky/basic.vert", "sky/basic.frag");

    if (!ShaderManager::Get("worldShader"))
        ShaderManager::Load("worldShader", "voxel/basic.vert", "voxel/basic.frag");

    }

MinecraftScene::~MinecraftScene()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
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

bool MinecraftScene::raycastShoot(float maxDistance, glm::ivec3& hitBlockPos, glm::ivec3& hitNormal) {
    
    const auto& camera = objects[0]->getComponent<CameraComponent>();
    glm::vec3 origin = camera->getPosition();
    glm::vec3 direction = glm::normalize(camera->getFront());

    const float stepSize = 0.1f;
    float distanceTraveled = 0.0f;

    glm::vec3 currentPos = origin;

    while (distanceTraveled < maxDistance) {
        glm::ivec3 blockPos = glm::floor(currentPos);
        BlockType block = world.getBlock(blockPos.x, blockPos.y, blockPos.z);

        if (block != BlockType::Air && block != BlockType::Water) {
            hitBlockPos = blockPos;

            // Estimate the normal by checking which axis had the largest step
            glm::vec3 prevPos = currentPos - direction * stepSize;
            glm::ivec3 prevBlock = glm::floor(prevPos);
            hitNormal = glm::clamp(blockPos - prevBlock, -1, 1);

            return true;
        }

        currentPos += direction * stepSize;
        distanceTraveled += stepSize;
    }

    return false;
}


void MinecraftScene::update(float dt)
{
    // Reduce update frequency for expensive operations
    static float chunkUpdateTimer = 0.0f;
    chunkUpdateTimer += dt;
    
    if (!isFirstTime){
    input->update(window);
    if (input->wasKeyPressed(GLFW_KEY_ESCAPE) || input->wasKeyPressed(GLFW_KEY_P)) {
        isPaused = !isPaused;
        if (isPaused) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (auto camera = objects[0]->getComponent<CameraComponent>()) {
                camera->setFirstMouse(true);
            }
        }
    }}
        if (oldHorizontalRadius != gTerrainSettings.horizontalRadius){
            // Reset lastCenter to force chunk reload on render distance change
            world.forceChunkReload();
            glm::vec3 pos = objects[0]->getComponent<CameraComponent>()->getPosition();
            world.ensureChunksNear(pos);
        }
        oldHorizontalRadius = gTerrainSettings.horizontalRadius;
        
    if (!isPaused){

        float speed = 5.0f * dt;
        const auto& camera = objects[0]->getComponent<CameraComponent>();
        glm::vec3 pos = camera->getPosition();

        glm::vec3 front = camera->getFront();
        glm::vec3 up = camera->getUp();
        glm::vec3 right = glm::normalize(glm::cross(up, front));

        glm::vec3 frontHorizontal = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 rightHorizontal = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

        glm::vec3 cameraDelta = glm::vec3(0.0f);        if (input->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
            speed *= 2.0f;
        }

        // Handle crouching
        bool wasIsCrouching = isCrouching;
        isCrouching = input->isKeyDown(GLFW_KEY_LEFT_SHIFT) && !isUnderWater;
        
        if (isCrouching) {
            speed *= crouchSpeedMultiplier;
            camera->setPlayerEyeHeight(crouchEyeHeight);
        } else {
            camera->setPlayerEyeHeight(normalEyeHeight);
        }

        if (input->isKeyDown(GLFW_KEY_W)) cameraDelta += speed * frontHorizontal;
        if (input->isKeyDown(GLFW_KEY_S)) cameraDelta -= speed * frontHorizontal;
        if (input->isKeyDown(GLFW_KEY_A)) cameraDelta += speed * rightHorizontal;
        if (input->isKeyDown(GLFW_KEY_D)) cameraDelta -= speed * rightHorizontal;

        if (input->wasKeyPressed(GLFW_KEY_1)) {
            selectedBlockIndex = 0;
        }
        if (input->wasKeyPressed(GLFW_KEY_2)) {
            selectedBlockIndex = 1;
        }
        if (input->wasKeyPressed(GLFW_KEY_3)) {
            selectedBlockIndex = 2;
        }
        if (input->wasKeyPressed(GLFW_KEY_4)) {
            selectedBlockIndex = 3;
        }
        selectedBlock = availableBlocks[selectedBlockIndex];
        if (input->wasMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::ivec3 hitBlock, hitNormal;
            if (raycastShoot(6.0f, hitBlock, hitNormal)) {
                // Example: remove the block
                world.setBlock(hitBlock.x, hitBlock.y, hitBlock.z, BlockType::Air);
            }
        }        if (input->wasMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::ivec3 hitBlock, hitNormal;
            if (raycastShoot(6.0f, hitBlock, hitNormal)) {
                glm::ivec3 placePos = hitBlock + hitNormal * glm::ivec3(-1);
                  // Check if placing a block at this position would collide with the player
                bool wouldCollideWithPlayer = false;
                
                // Player dimensions (same as collision system)
                const float playerWidth = 0.6f;
                const float currentPlayerHeight = camera->getPlayerEyeHeight(); // Use current eye height
                const float collisionBuffer = 0.05f;
                
                // Calculate player's current bounding box (feet-based position)
                glm::vec3 basePos = pos - glm::vec3(0.0f, camera->getPlayerEyeHeight(), 0.0f);
                glm::vec3 playerCenter = basePos + glm::vec3(0.0f, currentPlayerHeight * 0.5f, 0.0f);
                glm::vec3 playerMin = playerCenter - glm::vec3(playerWidth * 0.5f + collisionBuffer, currentPlayerHeight * 0.5f + collisionBuffer, playerWidth * 0.5f + collisionBuffer);
                glm::vec3 playerMax = playerCenter + glm::vec3(playerWidth * 0.5f + collisionBuffer, currentPlayerHeight * 0.5f + collisionBuffer, playerWidth * 0.5f + collisionBuffer);
                
                // Block bounding box at the place position
                glm::vec3 blockMin(static_cast<float>(placePos.x), static_cast<float>(placePos.y), static_cast<float>(placePos.z));
                glm::vec3 blockMax = blockMin + glm::vec3(1.0f);
                
                // Check if player bounding box intersects with the block position
                wouldCollideWithPlayer = (playerMin.x < blockMax.x && playerMax.x > blockMin.x) &&
                                       (playerMin.y < blockMax.y && playerMax.y > blockMin.y) &&
                                       (playerMin.z < blockMax.z && playerMax.z > blockMin.z);
                
                // Only place the block if it wouldn't collide with the player
                if (!wouldCollideWithPlayer) {
                    world.setBlock(placePos.x, placePos.y, placePos.z, selectedBlock);
                }
            }
        }// Calculate positions for water detection
        glm::vec3 feetPos = pos - glm::vec3(0.0f, camera->getPlayerEyeHeight(), 0.0f);
        glm::vec3 lowerBodyPos = pos - glm::vec3(0.0f, camera->getPlayerEyeHeight() * 0.5f, 0.0f);

        BlockType lowerBodyBlock = world.getBlock(
            static_cast<int>(std::floor(lowerBodyPos.x)),
            static_cast<int>(std::floor(lowerBodyPos.y)),
            static_cast<int>(std::floor(lowerBodyPos.z))
        );

        BlockType feetBlock = world.getBlock(
            static_cast<int>(std::floor(feetPos.x)),
            static_cast<int>(std::floor(feetPos.y)),
            static_cast<int>(std::floor(feetPos.z))
        );

        BlockType eyeBodyBlock = world.getBlock(
            static_cast<int>(std::floor(pos.x)),
            static_cast<int>(std::floor(pos.y)),
            static_cast<int>(std::floor(pos.z))
        );

        bool isLowerBodyUnderWater = (lowerBodyBlock == BlockType::Water);
        bool isFeetUnderWater = (feetBlock == BlockType::Water);
        isHeadUnderWater = (eyeBodyBlock == BlockType::Water);

        isUnderWater = isLowerBodyUnderWater;        if (input->wasKeyPressed(GLFW_KEY_SPACE)) {
            if (isOnGround && !isFeetUnderWater && !isCrouching) {
                velocity.y = jumpSpeed;
                isOnGround = false;
            } else if (!isHeadUnderWater && isFeetUnderWater && !isCrouching) {
                velocity.y = jumpSpeed * 1.1f;
                isOnGround = false;
            }
        }


        // Water physics
        if (isUnderWater) {
            gravity = -2.0f; // gentle sink

            if (input->isKeyDown(GLFW_KEY_SPACE)) {
                velocity.y += 5.0f * dt;
            }
            if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
                velocity.y -= 5.0f * dt;
            }

            velocity.y = std::clamp(velocity.y, -3.0f, 3.0f); // clamp to prevent extreme speeds
        } else {
            gravity = -10.8f;
        }

        velocity.y += gravity * dt;
        cameraDelta.y += velocity.y * dt;

        bool inputMoving = input->isKeyDown(GLFW_KEY_W) || input->isKeyDown(GLFW_KEY_A) ||
                        input->isKeyDown(GLFW_KEY_S) || input->isKeyDown(GLFW_KEY_D);
        isMoving = (inputMoving && isOnGround);        // Predict new position
        glm::vec3 newPos = pos + cameraDelta;
          // Calculate camera base position (feet position) first
        glm::vec3 basePos = pos - glm::vec3(0.0f, camera->getPlayerEyeHeight(), 0.0f) - prevBobOffsetVec;
        
        // Only update chunks every few frames to reduce overhead
        if (chunkUpdateTimer >= 0.2f) { // Update chunks every 200ms instead of every frame
            world.ensureChunksNear(basePos);
            chunkUpdateTimer = 0.0f;
        }
        glm::vec3 attemptedBasePos = basePos + cameraDelta;        // Player collision box (slightly smaller than a full block)
        const float playerWidth = 0.6f;  // Player width in blocks
        const float currentPlayerHeight = camera->getPlayerEyeHeight(); // Use current eye height for collision
        const float collisionBuffer = 0.05f; // Small buffer to prevent clipping through blocks
        const glm::vec3 playerHalfExtents(playerWidth * 0.5f + collisionBuffer, currentPlayerHeight * 0.5f + collisionBuffer, playerWidth * 0.5f + collisionBuffer);        // Function to check if a position collides with blocks
        auto checkCollision = [&](const glm::vec3& testPos) -> bool {
            // Get the player's bounding box at the test position
            glm::vec3 playerCenter = testPos + glm::vec3(0.0f, currentPlayerHeight * 0.5f, 0.0f);
            glm::vec3 minPos = playerCenter - playerHalfExtents;
            glm::vec3 maxPos = playerCenter + playerHalfExtents;

            // Optimized: Only check blocks that could intersect with player
            int minBlockX = static_cast<int>(std::floor(minPos.x));
            int maxBlockX = static_cast<int>(std::floor(maxPos.x));
            int minBlockY = static_cast<int>(std::floor(minPos.y));
            int maxBlockY = static_cast<int>(std::floor(maxPos.y));
            int minBlockZ = static_cast<int>(std::floor(minPos.z));
            int maxBlockZ = static_cast<int>(std::floor(maxPos.z));

            // Early exit: if bounding box is entirely above maximum terrain, no collision
            if (minBlockY > 100) return false; // Adjust based on your max terrain height

            for (int x = minBlockX; x <= maxBlockX; x++) {
                for (int y = minBlockY; y <= maxBlockY; y++) {
                    for (int z = minBlockZ; z <= maxBlockZ; z++) {
                        BlockType block = world.getBlock(x, y, z);
                        if (block != BlockType::Air && block != BlockType::Water) {
                            // Use more precise AABB intersection test
                            glm::vec3 blockMin(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                            glm::vec3 blockMax = blockMin + glm::vec3(1.0f);

                            bool intersects = (minPos.x < blockMax.x && maxPos.x > blockMin.x) &&
                                            (minPos.y < blockMax.y && maxPos.y > blockMin.y) &&
                                            (minPos.z < blockMax.z && maxPos.z > blockMin.z);

                            if (intersects) return true;
                        }
                    }
                }
            }
            return false;
        };

        // Test movement along each axis separately
        glm::vec3 finalPos = basePos;        // Edge prevention function for crouching
        auto wouldFallOffEdge = [&](const glm::vec3& testPos) -> bool {
            if (!isCrouching || !isOnGround) return false;
            
            // Check if there's a block at the same Y level that connects the current position to the new position
            int currentBlockY = static_cast<int>(std::floor(basePos.y - 0.1f)); // Block the player is standing on
            int newBlockX = static_cast<int>(std::floor(testPos.x));
            int newBlockZ = static_cast<int>(std::floor(testPos.z));
            
            // Check if there's a solid block at the same Y level at the new position
            BlockType blockAtNewPos = world.getBlock(newBlockX, currentBlockY, newBlockZ);
            return (blockAtNewPos == BlockType::Air || blockAtNewPos == BlockType::Water);
        };

        // Test X movement with edge prevention
        glm::vec3 testX = basePos;
        testX.x = attemptedBasePos.x;
        if (!checkCollision(testX) && !wouldFallOffEdge(testX)) {
            finalPos.x = attemptedBasePos.x;
        }

        // Test Z movement with edge prevention
        glm::vec3 testZ = finalPos;
        testZ.z = attemptedBasePos.z;
        if (!checkCollision(testZ) && !wouldFallOffEdge(testZ)) {
            finalPos.z = attemptedBasePos.z;
        }

        // Handle vertical movement and ground collision
        glm::vec3 testY = finalPos;
        testY.y = attemptedBasePos.y;
        
        if (!checkCollision(testY)) {
            // No collision - allow the movement
            finalPos.y = attemptedBasePos.y;
            isOnGround = false;
        } else {
            // Collision detected - handle ground collision
            if (velocity.y <= 0) {
                // Player is falling or stationary - find the ground level
                float groundY = finalPos.y;
                
                // Search upward to find the highest non-colliding position
                for (float testHeight = finalPos.y; testHeight < finalPos.y + 2.0f; testHeight += 0.1f) {
                    glm::vec3 testGroundPos = finalPos;
                    testGroundPos.y = testHeight;
                    
                    if (!checkCollision(testGroundPos)) {
                        groundY = testHeight;
                        break;
                    }
                }
                
                finalPos.y = groundY;
                isOnGround = true;
                velocity.y = 0;
            } else {
                // Player is jumping up - stop at current position
                velocity.y = 0;
            }
        }

        basePos = finalPos;
    // Handle head bobbing
    bobOffsetVec = glm::vec3(0.0f);
    cameraBasePosition = basePos;
    
    if (isMoving) {
        bobTime += dt * bobSpeed;
        float bobOffset = sin(bobTime) * bobAmount;
        bobOffsetVec = camera->getUp() * bobOffset;
    } else{  
        bobTime = 0.0f;
    }
    prevBobOffsetVec = bobOffsetVec;
    
    // Set final camera position: base position + eye height + bob offset
    camera->setPosition(cameraBasePosition + glm::vec3(0.0f, camera->getPlayerEyeHeight(), 0.0f) + bobOffsetVec);
    world.ensureChunksNear(basePos);

    }

    if (auto camera = objects[0]->getComponent<CameraComponent>()) {
        camera->setIsActive(!isPaused);
    }
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
    // if (isUnderWater) {
    //     glDisable(GL_DEPTH_TEST);
    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //     // drawFullScreenQuad(glm::vec4(0.0f, 0.4f, 0.7f, 0.4f)); // blue tint

    //     glDisable(GL_BLEND);
    //     glEnable(GL_DEPTH_TEST);
    // }
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!ShaderManager::Get("worldShader")) {
        std::cerr << "worldShader not loaded" << '\n';
        return;
    }
    const auto shader = ShaderManager::Get("worldShader");
    shader->use();
    shader->setBool("uIsUnderwater", isHeadUnderWater);

    for (const auto& object : objects)
        object->render(context);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::Begin("Selected Block");
    std::stringstream ss;
    ss << selectedBlock;
    ImGui::Text("Selected Block: %s", ss.str().c_str());   
    ImGui::End();



    if (isFirstTime) {
        renderStartMenu();
    }else if (isPaused) {
        renderPauseMenu();
    }
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (!isFirstTime && !isPaused) {
        if (crosshairRenderer) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        crosshairRenderer->render(screenWidth, screenHeight);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        }
    }


}

void MinecraftScene::renderPauseMenu() {
    if (!isPaused or isFirstTime) return;
    
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

    if (ImGui::Begin("StartMenu", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoInputs | 
        ImGuiWindowFlags_NoSavedSettings)) 
    {
        ImGui::End();
    }

    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);  // assuming Fonts[1] is bigger; load a bigger font before

    if (ImGui::Begin("Pause Menu", nullptr,
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoMove)) 
    {
        // Push your Minecraft font here to apply to everything inside this window

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Paused").x) * 0.5f);
        ImGui::Text("Paused");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Resume", ImVec2(-FLT_MIN, 0))) {
            isPaused = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (auto camera = objects[0]->getComponent<CameraComponent>()) {
                camera->setFirstMouse(true);
            }
        }

        if (ImGui::Button("Exit", ImVec2(-FLT_MIN, 0))) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } 
          ImGui::SliderFloat("Base Noise Frequency", &gTerrainSettings.noiseFrequency, 0.0001f, 0.1f, "%.5f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Mountain Noise Frequency", &gTerrainSettings.mountainFrequency, 0.0005f, 0.1f, "%.5f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderInt("Max Terrain Height", &gTerrainSettings.maxTerrainHeight, 16, 256);
        ImGui::SliderInt("Water Level", &gTerrainSettings.waterLevel, 0, 128);
        ImGui::SliderInt("Render Distance", &gTerrainSettings.horizontalRadius, 2, 32, "%d chunks");


        if (ImGui::Button("Regenerate World", ImVec2(-FLT_MIN, 0))) {
            world.generate(); // or your own method to regenerate terrain
            if (auto camera = objects[0]->getComponent<CameraComponent>()) {
                camera->setPosition(glm::vec3(0.0f, 70.0f, 3.0f));
            }
        }
        
        if (ImGui::Button("Reposition player upwards", ImVec2(-FLT_MIN, 0))) {
            if (auto camera = objects[0]->getComponent<CameraComponent>()) {
                camera->setPosition(camera->getPosition() + glm::vec3(0.0f, 70.0f, 0.0f));
            }
        }

        ImGui::PopFont();  // pop the Minecraft font here

        ImGui::End();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

}

void MinecraftScene::renderStartMenu() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

    if (ImGui::Begin("StartMenu", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoInputs | 
        ImGuiWindowFlags_NoSavedSettings)) 
    {
        ImGui::End();
    }

    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 400));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);  // assuming Fonts[1] is bigger; load a bigger font before

    if (ImGui::Begin("Start Menu", nullptr,
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoMove)) 
    {
        // Push your Minecraft font here to apply to everything inside this window

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Not Minecraft hehe").x) * 0.5f);
        ImGui::Text("Not Minecraft hehe");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Start", ImVec2(-FLT_MIN, 0))) {
            isPaused = false;
            isFirstTime = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (auto camera = objects[0]->getComponent<CameraComponent>()) {
                camera->setFirstMouse(true);
            }
        }

        if (ImGui::Button("Exit", ImVec2(-FLT_MIN, 0))) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        ImGui::PopFont();  // pop the Minecraft font here

        ImGui::End();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

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
