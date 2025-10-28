#include "TestDefinitions.h"

namespace LaurelEye {
    // ------------------------------------------------------------
    // Engine lifecycle test (basic run-stop flow)
    // ------------------------------------------------------------
    void engineLifecycleTest() {
        std::cout << "[EngineTest] Starting Engine Lifecycle Test...\n";

        EngineConfig config;
        config.projectName = "LifecycleTest";
        config.enableDebugMode = true;
        config.assetRoot = "assets/";
        config.initialSceneList = "scenes/scene_list.json";

        Engine engine(config);

        // Run engine in a separate thread since run() blocks until stop() is called.
        std::thread engineThread([&engine]() {
            engine.run();
        });

        // Let it run a few frames
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        engine.stop();
        engineThread.join();

        std::cout << "[EngineTest] Engine ran and stopped cleanly.\n";
        std::cout << "[EngineTest] Passed Engine Lifecycle Test\n";
    }

    // ------------------------------------------------------------
    // EngineContext service registration and retrieval
    // ------------------------------------------------------------
    struct DummyService {
        int value = 42;
    };

    void engineContextServiceTest() {
        std::cout << "[EngineTest] Starting EngineContext Service Test...\n";

        EngineContext context;

        auto dummy = std::make_unique<DummyService>();
        context.registerService<DummyService>(dummy.get());

        auto retrieved = context.getService<DummyService>();
        assert(retrieved);
        assert(retrieved->value == 42);

        std::cout << "[EngineTest] EngineContext correctly registered and retrieved service.\n";
        std::cout << "[EngineTest] Passed EngineContext Service Test\n";
    }

     // ------------------------------------------------------------
    // SystemCoordinator Wiring Test (using real TransformSystem)
    // ------------------------------------------------------------
    void engineSystemCoordinatorWiringTest() {
        std::cout << "[EngineTest] Starting SystemCoordinator Wiring Test...\n";

        EngineContext context;
        EngineConfig config;
        ResourceCoordinator resourceCoord(context, config);
        SystemCoordinator sysCoord(context, config);

        resourceCoord.initialize();
        sysCoord.initialize();
        
        auto transformSys = context.getService<TransformSystem>();
        assert(transformSys && "TransformSystem should be registered in EngineContext after initialization");

        sysCoord.update(0.016f);
        sysCoord.shutdown();

        std::cout << "[EngineTest] Passed SystemCoordinator Wiring Test\n";
    }

    // ------------------------------------------------------------
    // Overwrite Service Test
    // ------------------------------------------------------------
    void engineContextOverwriteTest() {
        EngineContext ctx;

        DummyService s1;
        s1.value = 10;
        ctx.registerService<DummyService>(&s1);

        DummyService s2;
        s2.value = 99;
        ctx.registerService<DummyService>(&s2);

        auto* retrieved = ctx.getService<DummyService>();
        assert(retrieved == &s2 && "Service pointer should be overwritten with the new one");
        assert(retrieved->value == 99 && "Service should reflect the new instance");

        std::cout << "[EngineTest] Passed Engine Context Overwrite Test\n";
    }

}// namespace LaurelEye
