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
    };

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

            if ( inputManager.isKeyPressed(LaurelEye::Key::Escape) ) {
                return 0; // bail out of program early
            }
        }
    }
    std::cout << "Tests are finished, moving to gameplay demo" << std::endl;

    // gameplay test
    //struct GameplayScene : public LaurelEye::Scene {
    //    explicit GameplayScene(const std::string& name)
    //        : Scene(name) {}

    //protected:
    //    void OnEnter() override {
    //        std::cout << "Entering scene: " << getName() << std::endl;
    //    }

    //    void OnExit() override {
    //        std::cout << "Exiting scene: " << getName() << std::endl;
    //    }

    //    void OnResume() override {
    //        std::cout << "Resuming scene: " << getName() << std::endl;
    //    }

    //    void OnPause() override {
    //        std::cout << "Pausing scene: " << getName() << std::endl;
    //    }
    //};

    //std::unique_ptr<GameplayScene> gameplayScene = std::make_unique<GameplayScene>("GameplayScene");
    //std::unique_ptr<LaurelEye::Entity> player = std::make_unique<LaurelEye::Entity>("Player");
    //player->addTag("Player");
    //gameplayScene->addEntity({std::move(player), nullptr});

    //while ( !inputSystem.isKeyPressed(LaurelEye::Key::Escape) ) {
    //    glfwP.update();
    //    inputSystem.update();
    //    float deltaTime = 0.016f; // ~60 FPS
    //    gameplayScene->update(deltaTime);

    //    // For now just know when keys are pressed
    //    if ( inputSystem.isKeyPressed(LaurelEye::Key::W) )
    //        std::cout << "W key pressed!" << std::endl;
    //    if ( inputSystem.isKeyPressed(LaurelEye::Key::A) )
    //        std::cout << "A key pressed!" << std::endl;
    //    if ( inputSystem.isKeyPressed(LaurelEye::Key::S) )
    //        std::cout << "A key pressed!" << std::endl;
    //    if ( inputSystem.isKeyPressed(LaurelEye::Key::D) )
    //        std::cout << "A key pressed!" << std::endl;

    //    if ( inputSystem.isKeyHeld(LaurelEye::Key::W) )
    //        std::cout << "W key Held!" << std::endl;
    //    if ( inputSystem.isKeyHeld(LaurelEye::Key::A) )
    //        std::cout << "A key Held!" << std::endl;
    //    if ( inputSystem.isKeyHeld(LaurelEye::Key::S) )
    //        std::cout << "A key Held!" << std::endl;
    //    if ( inputSystem.isKeyHeld(LaurelEye::Key::D) )
    //        std::cout << "A key Held!" << std::endl;

    //    if ( inputSystem.isKeyReleased(LaurelEye::Key::W) )
    //        std::cout << "W key Released!" << std::endl;
    //    if ( inputSystem.isKeyReleased(LaurelEye::Key::A) )
    //        std::cout << "A key Released!" << std::endl;
    //    if ( inputSystem.isKeyReleased(LaurelEye::Key::S) )
    //        std::cout << "A key Released!" << std::endl;
    //    if ( inputSystem.isKeyReleased(LaurelEye::Key::D) )
    //        std::cout << "A key Released!" << std::endl;

    //    if ( inputSystem.isMouseButtonPressed(LaurelEye::MouseButton::Left) )
    //        std::cout << "Left Click pressed!" << std::endl;
    //    if ( inputSystem.isMouseButtonPressed(LaurelEye::MouseButton::Right) )
    //        std::cout << "Right Click pressed!" << std::endl;

    //    if ( inputSystem.isMouseButtonHeld(LaurelEye::MouseButton::Left) )
    //        std::cout << "Left Click Held!" << std::endl;
    //    if ( inputSystem.isMouseButtonHeld(LaurelEye::MouseButton::Right) )
    //        std::cout << "Right Click Held!" << std::endl;

    //    if ( inputSystem.isMouseButtonReleased(LaurelEye::MouseButton::Left) )
    //        std::cout << "Left Click Released!" << std::endl;
    //    if ( inputSystem.isMouseButtonReleased(LaurelEye::MouseButton::Right) )
    //        std::cout << "Right Click Released!" << std::endl;

    //    // Sleep or wait a bit to avoid spamming the console
    //    glfwWaitEventsTimeout(0.05);
    //}
    wm.shutdown();
    glfwP.shutdown();
    return 0;
}
