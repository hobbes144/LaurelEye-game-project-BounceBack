#include "TestDefinitions.h"

namespace LaurelEye {
    class MockWindow : public IWindow {
    public:
        MockWindow() {
            attributes.width = 800;
            attributes.height = 600;
            attributes.title = "Mock Window";
        }

        // --- Core virtuals ---
        bool shouldClose() const override {
            return false;
        }

        NativeWindowHandle getNativeHandle() override {
            // Just return nullptr since we don't need a real window
            return nullptr;
        }

        double getTime() override {
            static double fakeTime = 0.0;
            fakeTime += 0.016; // Simulate ~60 FPS ticking
            return fakeTime;
        }

        // --- Setters ---
        void setTitle(std::string _title) override {
            attributes.title = std::move(_title);
        }

        void setWidth(int _width) override {
            attributes.width = _width;
        }

        void setHeight(int _height) override {
            attributes.height = _height;
        }

        void setVsync(bool flag) override {
            attributes.vsync = flag;
        }

        void setFullscreen(bool flag) override {
            // attributes.fullscreen = flag;
        }
    };
    void scriptingBasicTest() {
        using namespace Scripting;

        std::cout << "[ScriptTest] Basic script execution...\n";

        // Create ScriptSystem with Sol2 backend
        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        auto window = std::make_shared<MockWindow>();
        auto inputManager = std::make_shared<InputManager>(*window);
        context->registerService<InputManager>(inputManager.get());

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        // Create a dummy entity and attach a ScriptComponent
        Entity entity;
        auto* scriptComp = entity.addComponent<ScriptComponent>(std::string(TEST_MEDIA_DIR) + "/scripts/test.lua");

        scriptSystem.registerComponent(scriptComp);
        scriptSystem.update(0.016f); // simulate one frame

        // Check the Lua global was set
        auto* sol2State = dynamic_cast<Sol2State*>(scriptSystem.getEngineState());
        assert(sol2State && "Failed to get Sol2State");

        auto* scriptInstance = dynamic_cast<Sol2ScriptInstance*>(scriptComp->getScriptInstance());
        assert(scriptInstance);
        auto called = scriptInstance->getVar<bool>("called");
        assert(called && *called);

        scriptSystem.shutdown();
        std::cout << "[ScriptTest] Passed" << std::endl;
    }

    void scriptingMultipleTest() {
        using namespace Scripting;
        std::cout << "[ScriptTest] Multiple components...\n";

        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        auto window = std::make_shared<MockWindow>();
        auto inputManager = std::make_shared<InputManager>(*window);
        context->registerService<InputManager>(inputManager.get());

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        Entity e1, e2;
        auto* c1 = e1.addComponent<ScriptComponent>(std::string(TEST_MEDIA_DIR) + "/scripts/test_multi.lua");
        auto* c2 = e2.addComponent<ScriptComponent>(std::string(TEST_MEDIA_DIR) + "/scripts/test_multi.lua");

        scriptSystem.registerComponent(c1);
        scriptSystem.registerComponent(c2);

        scriptSystem.update(0.016f);
        scriptSystem.update(0.016f);

        auto* sol2 = dynamic_cast<Sol2State*>(scriptSystem.getEngineState());
        assert(sol2);

        auto* scriptInstance = dynamic_cast<Sol2ScriptInstance*>(c1->getScriptInstance());
        assert(scriptInstance);
        auto count = scriptInstance->getVar<int>("count");
        assert(count == 3 && "Expected count == 3 (1 init + 2 updates)");

        scriptSystem.shutdown();
        std::cout << "[ScriptTest] Passed\n";
    }

    void scriptingAPITest() {
        using namespace Scripting;
        std::cout << "[ScriptTest] API registration test...\n";

        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        auto window = std::make_shared<MockWindow>();
        auto inputManager = std::make_shared<InputManager>(*window);
        context->registerService<InputManager>(inputManager.get());

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        // Simulate that Time.deltaTime was updated by the engine
        auto* sol2State = dynamic_cast<Sol2State*>(scriptSystem.getEngineState());
        assert(sol2State);

        Entity entity("APIEntity");
        auto* scriptComp = entity.addComponent<ScriptComponent>(
            std::string(TEST_MEDIA_DIR) + "/scripts/test_lifecycle.lua");

        scriptSystem.registerComponent(scriptComp);
        scriptSystem.update(0.016f);

        auto* scriptInstance = dynamic_cast<Sol2ScriptInstance*>(scriptComp->getScriptInstance());
        assert(scriptInstance);

        auto called = scriptInstance->getVar<bool>("called");
        assert(called && *called && "Expected 'called' to be set by script");

        auto inputPressed = scriptInstance->getVar<bool>("inputPressed");
        assert(inputPressed && *inputPressed && "Expected Input.isPressed(32) to return true");

        auto deltaTimeVal = scriptInstance->getVar<float>("deltaTimeVal");
        assert(deltaTimeVal && *deltaTimeVal == 0.016f && "Expected Time.deltaTime to be set correctly");

        scriptSystem.shutdown();
        std::cout << "[ScriptTest] Passed\n";
    }

    void scriptingMathTest() {
        using namespace Scripting;
        std::cout << "[ScriptTest] Math + Transform API test...\n";

        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        auto window = std::make_shared<MockWindow>();
        auto inputManager = std::make_shared<InputManager>(*window);
        context->registerService<InputManager>(inputManager.get());

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        Entity entity("MathEntity");
        auto* scriptComp = entity.addComponent<ScriptComponent>(
            std::string(TEST_MEDIA_DIR) + "/scripts/test_math.lua");

        scriptSystem.registerComponent(scriptComp);
        scriptSystem.update(0.016f);

        auto* scriptInstance = dynamic_cast<Sol2ScriptInstance*>(scriptComp->getScriptInstance());
        assert(scriptInstance);

        // Vector3 addition
        auto vAddX = scriptInstance->getVar<float>("vAddX");
        auto vAddY = scriptInstance->getVar<float>("vAddY");
        auto vAddZ = scriptInstance->getVar<float>("vAddZ");
        assert(vAddX && vAddY && vAddZ);
        assert(*vAddX == 5.0f && *vAddY == 7.0f && *vAddZ == 9.0f);

        // Quaternion rotation
        auto qRotX = scriptInstance->getVar<float>("qRotX");
        auto qRotY = scriptInstance->getVar<float>("qRotY");
        auto qRotZ = scriptInstance->getVar<float>("qRotZ");
        assert(qRotX && qRotY && qRotZ);
        assert(std::abs(*qRotX - 1.0f) < 0.0001f);
        assert(std::abs(*qRotY - 0.0f) < 0.0001f);
        assert(std::abs(*qRotZ - 0.0f) < 0.0001f);

        // Matrix direction transform
        auto mDirX = scriptInstance->getVar<float>("mDirX");
        auto mDirY = scriptInstance->getVar<float>("mDirY");
        auto mDirZ = scriptInstance->getVar<float>("mDirZ");
        assert(mDirX && mDirY && mDirZ);
        assert(std::abs(*mDirX - 1.0f) < 0.0001f);
        assert(std::abs(*mDirY - 0.0f) < 0.0001f);
        assert(std::abs(*mDirZ - 0.0f) < 0.0001f);

        // Transform composition
        auto t3PosX = scriptInstance->getVar<float>("t3PosX");
        auto t3PosY = scriptInstance->getVar<float>("t3PosY");
        auto t3PosZ = scriptInstance->getVar<float>("t3PosZ");
        assert(t3PosX && t3PosY && t3PosZ);
        assert(std::abs(*t3PosX - 3.0f) < 0.0001f);
        assert(std::abs(*t3PosY - 0.0f) < 0.0001f);
        assert(std::abs(*t3PosZ - 0.0f) < 0.0001f);

        // Transform direction check
        auto t3DirX = scriptInstance->getVar<float>("t3DirX");
        auto t3DirY = scriptInstance->getVar<float>("t3DirY");
        auto t3DirZ = scriptInstance->getVar<float>("t3DirZ");
        assert(t3DirX && t3DirY && t3DirZ);
        assert(std::abs(*t3DirX - 1.0f) < 0.0001f);
        assert(std::abs(*t3DirY - 0.0f) < 0.0001f);
        assert(std::abs(*t3DirZ - 0.0f) < 0.0001f);

        scriptSystem.shutdown();
        std::cout << "[ScriptTest] Passed\n";
    }

    void scriptingInputTest(GlfwPlatform* glfwP, InputManager* pInputManager) {
        using namespace LaurelEye;
        using namespace Scripting;

        std::cout << "------- Scripting Input Test -------" << std::endl;

        // === Initialize Script System ===
        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        context->registerService<InputManager>(pInputManager);

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        // === Create Entity with Script ===
        Entity entity("InputEntity");
        auto* scriptComp = entity.addComponent<ScriptComponent>(
            std::string(TEST_MEDIA_DIR) + "/scripts/test_input.lua");
        scriptSystem.registerComponent(scriptComp);

        // === Main Loop ===
        std::cout << "Press keys to test scripting input. Escape to exit.\n";

        float dt = 1.0f / 60.0f;
        while ( !pInputManager->isKeyPressed(Key::Escape) ) {
            glfwP->update();
            pInputManager->update();
            scriptSystem.update(dt);

            // Pull some test vars from Lua to verify it reacts to real input
            auto* scriptInstance = dynamic_cast<Sol2ScriptInstance*>(scriptComp->getScriptInstance());
            assert(scriptInstance);

            auto wPressed = scriptInstance->getVar<bool>("wPressed");
            auto aPressed = scriptInstance->getVar<bool>("aPressed");
            auto sPressed = scriptInstance->getVar<bool>("sPressed");
            auto dPressed = scriptInstance->getVar<bool>("dPressed");

            if ( wPressed && *wPressed ) std::cout << "Lua detected W pressed\n";
            if ( aPressed && *aPressed ) std::cout << "Lua detected A pressed\n";
            if ( sPressed && *sPressed ) std::cout << "Lua detected S pressed\n";
            if ( dPressed && *dPressed ) std::cout << "Lua detected D pressed\n";

            // Sleep to avoid spamming the console
            glfwWaitEventsTimeout(0.05);
        }

        // === Cleanup ===
        scriptSystem.shutdown();

        std::cout << "------- Scripting Input Test End -------" << std::endl;
    }

    void scriptingECSTest() {
        using namespace LaurelEye;
        using namespace Scripting;

        std::cout << "------- Scripting ECS Test -------" << std::endl;

        // === Initialize Script System ===
        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        // === Create Entity ===
        Entity entity("ECS_Test_Entity");
        auto* transform = entity.addComponent<TransformComponent>();
        auto* scriptComp = entity.addComponent<ScriptComponent>(
            std::string(TEST_MEDIA_DIR) + "/scripts/test_ecs.lua");
        scriptSystem.registerComponent(scriptComp);

        // === Run Script Simulation ===
        float dt = 1.0f / 60.0f;
        for ( int i = 0; i < 5; ++i ) {
            scriptSystem.update(dt);
        }

        // === Verify that script modified the Transform ===
        if ( transform ) {
            auto pos = transform->getWorldPosition();
            std::cout << "Transform after script: ("
                      << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
        }

        // === Cleanup ===
        scriptSystem.shutdown();

        std::cout << "------- Scripting ECS Test End -------" << std::endl;
    }

    void scriptingSceneTest() {
#if !defined(NDEBUG)
        using namespace LaurelEye;
        using namespace Scripting;

        std::cout << "------- Scripting Scene Test -------" << std::endl;

        // 1. EngineContext + SceneManager
        std::shared_ptr<EngineContext> context = std::make_shared<EngineContext>();
        EngineConfig config{};
        config.assetRoot = TEST_MEDIA_DIR;

        auto sceneManager = std::make_unique<SceneManager>(*context, config);
        context->registerService<SceneManager>(sceneManager.get());

        // 2. Inject a simple test scene
        auto testScene = std::make_unique<Scene>("TestScene");
        {
            auto player = std::make_unique<Entity>("Player");
            auto light = std::make_unique<Entity>("Light");
            testScene->addEntity(std::move(player));
            testScene->addEntity(std::move(light));
        }

        sceneManager->injectSceneForTest("TestScene", std::move(testScene));
        sceneManager->setCurrentScene(sceneManager->getScene("TestScene"));

        sceneManager->getCurrentScene()->initialize(*context);

        // 3. Script system
        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
        scriptSystem.setEngineContext(*context);
        scriptSystem.initialize();

        // 4. Entity with Lua script
        Entity scriptEntity("SceneTestEntity");
        auto* scriptComp = scriptEntity.addComponent<ScriptComponent>(
            std::string(TEST_MEDIA_DIR) + "/scripts/test_scene.lua");
        scriptSystem.registerComponent(scriptComp);

        // 5. Update loop
        std::cout << "Running scene script...\n";
        const float dt = 1.0f / 60.0f;
        for ( int i = 0; i < 5; ++i ) {
            sceneManager->update(dt);
            scriptSystem.update(dt);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        scriptSystem.shutdown();
        std::cout << "------- Scripting Scene Test End -------" << std::endl;
#endif
    }
} // namespace LaurelEye
