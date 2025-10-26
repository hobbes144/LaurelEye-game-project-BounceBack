#include "TestDefinitions.h"

namespace LaurelEye {
    void scriptingBasicTest() {
        using namespace Scripting;

        std::cout << "[ScriptTest] Basic script execution...\n";

        // Create ScriptSystem with Sol2 backend
        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
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

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
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

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
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

        ScriptSystem scriptSystem(ScriptSystem::ScriptSystemType::Sol2);
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

}
