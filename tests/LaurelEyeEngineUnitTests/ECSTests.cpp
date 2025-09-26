#include "TestDefinitions.h"

namespace LaurelEye {
    void entityTest() {
        // Create some entities
        std::unique_ptr<LaurelEye::Entity> root = std::make_unique<LaurelEye::Entity>("RootEntity");
        std::unique_ptr<LaurelEye::Entity> child1 = std::make_unique<LaurelEye::Entity>("Child1");
        std::unique_ptr<LaurelEye::Entity> child2 = std::make_unique<LaurelEye::Entity>("Child2");

        // Add children
        root->addChild(std::move(child1));
        root->addChild(std::move(child2));

        // Test hierarchy
        auto found = root->findChildByName("Child1");
        std::cout << (found ? "Child1 Found" : "Child1 Not Found") << std::endl;

        // Test tags
        root->addTag("Player");
        std::cout << (root->compareTag("Player") ? "Tag Passed" : "Tag Failed") << std::endl;

        // Test components using dummy component
        struct DummyComponent : public LaurelEye::IComponent {
            void initialize() override { std::cout << "Initializing Dummy Component." << std::endl; }
            void update(float) override { std::cout << "Updating Dummy Component." << std::endl; }
            void shutdown() override { std::cout << "Shutting down Dummy Component." << std::endl; }
            void configure(const std::unordered_map<std::string, std::any>&) override {}
        };

        auto comp = root->addComponent<DummyComponent>();
        std::cout << ((comp != nullptr) ? "Component Added" : "Component Failed") << std::endl;

        auto foundComp = root->findComponent<DummyComponent>();
        std::cout << ((foundComp != nullptr) ? "Component Found" : "Component Missing") << std::endl;

        root->initialize();

        for ( int frame = 0; frame < 100; ++frame ) {
            float deltaTime = 0.016f; // ~60 FPS
            std::cout << "--- Frame " << frame << " ---" << std::endl;
            root->update(deltaTime);
        }
    }

    void sceneTest() {
        // Create some entities
        std::unique_ptr<LaurelEye::Entity> player = std::make_unique<LaurelEye::Entity>("Player");
        std::unique_ptr<LaurelEye::Entity> child1 = std::make_unique<LaurelEye::Entity>("Child1");
        std::unique_ptr<LaurelEye::Entity> child2 = std::make_unique<LaurelEye::Entity>("Child2");

        // Add children
        player->addChild(std::move(child1));
        player->addChild(std::move(child2));

        // Test tags
        player->addTag("Player");

        // Test components using dummy component
        struct ShootyComponent : public LaurelEye::IComponent {
            void initialize() override { std::cout << "Initializing ShootyComponent." << std::endl; }
            void update(float) override { std::cout << "Shooting the ShootyComponent" << std::endl; }
            void shutdown() override { std::cout << "Shutting down ShootyComponent." << std::endl; }
            void configure(const std::unordered_map<std::string, std::any>&) override {}
        };

        // Generate a test scene that does nothing but iterate
        struct TestScene : public LaurelEye::Scene {
            explicit TestScene(const std::string& name)
                : Scene(name) {}

        protected:
            void OnEnter() override {
                std::cout << "Entering scene: " << getName() << std::endl;
            }

            void OnExit() override {
                std::cout << "Exiting scene: " << getName() << std::endl;
            }

            void OnResume() override {
                std::cout << "Resuming scene: " << getName() << std::endl;
            }

            void OnPause() override {
                std::cout << "Pausing scene: " << getName() << std::endl;
            }
        };

        auto comp = player->addComponent<ShootyComponent>();

        std::unique_ptr<TestScene> testScene = std::make_unique<TestScene>("TestScene");
        testScene->addEntity({std::move(player), nullptr});

        for ( int frame = 0; frame < 100; ++frame ) {
            float deltaTime = 0.016f; // ~60 FPS
            std::cout << "--- Frame " << frame << " ---" << std::endl;
            testScene->update(deltaTime);
        }
    }
}
