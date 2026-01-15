#include "TestDefinitions.h"

namespace LaurelEye {
    void entityTest() {
    //    // Dummy component for testing
    //    struct DummyComponent : public IComponent {
    //        int value{42};
    //    };

    //    // Create entity
    //    auto root = std::make_unique<Entity>("RootEntity");
    //    assert(root != nullptr);
    //    assert(root->getName() == "RootEntity");

    //    // Check unique ID generation
    //    unsigned int rootId = root->getId();
    //    auto another = std::make_unique<Entity>("AnotherEntity");
    //    assert(another->getId() != rootId);

    //    // Tag tests
    //    root->addTag("Player");
    //    assert(root->compareTag("Player"));
    //    assert(!root->compareTag("Enemy"));
    //    std::cout << "Tag tests passed\n";

    //    // Component tests
    //    DummyComponent* comp = root->addComponent<DummyComponent>();
    //    assert(comp != nullptr);
    //    assert(comp->value == 42);

    //    DummyComponent* foundComp = root->findComponent<DummyComponent>();
    //    assert(foundComp != nullptr);
    //    assert(foundComp == comp);
    //    std::cout << "Component add/find passed\n";

    //    // Remove component and verify
    //    root->removeComponent<DummyComponent>();
    //    DummyComponent* removedComp = root->findComponent<DummyComponent>();
    //    assert(removedComp == nullptr);
    //    std::cout << "Component removal passed\n";
    }

    void sceneTest() {
    //    using namespace LaurelEye;

    //    std::cout << "------- Scene Test -------" << std::endl;

    //    // Dummy component for testing
    //    struct ShootyComponent : public IComponent {
    //        int bullets{50};
    //    };

    //    // === Setup Context & Dummy Scene ===
    //    EngineContext ctx;
    //    auto dummyJson = std::make_shared<IO::JsonAsset>("");
    //    dummyJson->jsonDocument.SetObject(); // empty

    //    std::string assetRoot = TEST_MEDIA_DIR;

    //    auto testScene = std::make_unique<Scene>("TestScene", ctx, dummyJson, assetRoot);
    //    assert(testScene->getName() == "TestScene");

    //    // === Entities ===
    //    auto player = std::make_unique<Entity>("Player");
    //    auto child1 = std::make_unique<Entity>("Child1");
    //    auto child2 = std::make_unique<Entity>("Child2");

    //    player->addTag("Player");
    //    child1->addTag("Ally");
    //    child2->addTag("Enemy");

    //    // Add component
    //    auto comp = player->addComponent<ShootyComponent>();
    //    assert(comp && comp->bullets == 50);

    //    // Add entities
    //    testScene->addEntity(std::move(player));
    //    testScene->addEntity(std::move(child1));
    //    testScene->addEntity(std::move(child2));

    //    // Before update: not spawned
    //    auto preUpdatePlayers = testScene->findEntitiesWithTag("Player");
    //    assert(preUpdatePlayers.empty());
    //    std::cout << "Pre-update: No Player Found\n";

    //    // Trigger spawn
    //    for ( int i = 0; i < 2; ++i ) {
    //        testScene->update(0.016f);
    //    }

    //    // After update
    //    auto postUpdatePlayers = testScene->findEntitiesWithTag("Player");
    //    assert(!postUpdatePlayers.empty());
    //    std::cout << "Post-update: Player Found\n";

    //    // Additional checks
    //    auto foundPlayer = testScene->findEntityByName("Player");
    //    assert(foundPlayer && foundPlayer->compareTag("Player"));

    //    auto allies = testScene->findEntitiesWithTag("Ally");
    //    auto enemies = testScene->findEntitiesWithTag("Enemy");
    //    assert(allies.size() == 1);
    //    assert(enemies.size() == 1);

    //    std::cout << "------- Scene Test End -------" << std::endl;
    }

} // namespace LaurelEye
