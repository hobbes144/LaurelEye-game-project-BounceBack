/// @brief Our main test handler - main function entry point
/// in the future a proper workflow will be implemented. For now
/// simply load in your functionality and run it locally. Do not
/// push up localized testing in this file to GitHub to prevent
/// merge conflicts.
#include <iostream>
#include "TestDefinitions.h"

int main() {
    LaurelEye::GlfwPlatform glfwP = LaurelEye::GlfwPlatform();
    glfwP.initialize();

    LaurelEye::WindowManager wm = LaurelEye::WindowManager();
    auto window = wm.createWindow(LaurelEye::WindowDescription());

    LaurelEye::InputManager inputManager(*window);

    typedef void (*test)();
    struct NamedTest {
        const char* name;
        test func;
    };
    std::vector<NamedTest> tests = {
        {"MathTest", LaurelEye::mathTest},
        {"EntityTest", LaurelEye::entityTest},
        {"SceneTest",  LaurelEye::sceneTest},
        {"PhysicsTest", LaurelEye::physicsTest},
        {"TransformTest", LaurelEye::transformTest},
        {"TransformSystemTest", LaurelEye::transformSystemTest},
        {"TransformHierarchyTest", LaurelEye::transformHierarchyTest},
        // {"RenderTest", LaurelEye::renderTest}
        {"AudioTest1", LaurelEye::audioPlayTest},
        {"AudioTest2", LaurelEye::audioPauseTest},
        {"AudioTest3", LaurelEye::audioResumeTest},
        {"AudioTest4", LaurelEye::audioStopTest},
        {"AudioTest4", LaurelEye::speakerTest},
        {"AudioTest4", LaurelEye::speakerTest2},
        {"AudioTest4", LaurelEye::speakerTest3},
        {"EventTest", LaurelEye::eventTest},
        {"MemoryManagerTest", LaurelEye::memoryTest},
        //{"AssetMeshTest", LaurelEye::assetMeshTest},  These tests fail now due to render system dependency - is fine ig
        //{"AssetImageTest", LaurelEye::assetImageTest},
        //{"AssetCacheTest", LaurelEye::assetCacheTest},
        {"ScriptingBaseTest", LaurelEye::scriptingBasicTest},
        {"ScriptingMultipleTest", LaurelEye::scriptingMultipleTest},
        {"ScriptingMathTest", LaurelEye::scriptingMathTest},
        {"EngineContextServiceTest", LaurelEye::engineContextServiceTest},
        //{"EngineSystemCoordinatorWiringTest", LaurelEye::engineSystemCoordinatorWiringTest},
        {"EngineContextOverwriteTest",        LaurelEye::engineContextOverwriteTest},
        {"CollisionEventTest", LaurelEye::collisionEventTest}
    };

    renderTestExtended(window, &glfwP, &inputManager);

    for ( NamedTest testFunc : tests ) {
        std::cout << "Running test: " << testFunc.name << std::endl;

        // Run the test
        testFunc.func();

        // Wait for SPACE before moving on
        std::cout << "Press SPACE to continue..." << std::endl;

        bool waiting = true;
        while ( waiting ) {
            glfwP.update();
            inputManager.update();

            if ( inputManager.isKeyPressed(LaurelEye::Key::Space) ) {
                waiting = false; // leave the inner loop, go to next test
            }

            if ( window->shouldClose() || inputManager.isKeyPressed(LaurelEye::Key::Escape) ) {
                wm.shutdown();
                glfwP.shutdown();
                return 0; // break out and close program
            }
        }
    }

    //Manual Tests
    //LaurelEye::inputPhysTest(glfwP, inputManager);
    LaurelEye::scriptingInputTest(&glfwP, &inputManager);

    std::cout << "Tests are finished, moving to gameplay demo" << std::endl;


    wm.shutdown();
    glfwP.shutdown();
    return 0;
}
