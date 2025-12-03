#include "TestDefinitions.h"

namespace LaurelEye {
    void ThreadingTest() {
        //EngineContext context;
        //LaurelEye::Graphics::RenderSystem renderSystem = LaurelEye::Graphics::RenderSystem();
        //LaurelEye::Graphics::RenderSystemConfig renderConfig;
        //renderSystem.setConfig(renderConfig);
        //renderSystem.initialize();
        //context.registerService<LaurelEye::Graphics::RenderSystem>(&renderSystem);
        //LaurelEye::IO::AssetManager manager(context);
        //manager.initialize();

        //// Simulated asset paths (can be real files or mocks)
        //std::vector<std::string> assets = {
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/textures/test.png",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/textures/roman_blue_clothes_texture.jpg",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/textures/viking_blue_C_texture.jpg",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/textures/roman_blue_D_texture.jpg",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/textures/viking_column_texture.jpg",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/models/egyptian_B.fbx",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/models/roman_D.fbx",
        //    "tests/LaurelEyeEngineUnitTests/TestMedia/models/viking_C.fbx",
        //};

        //// --- TEST 1: Sequential load (baseline) ---
        //auto startSync = std::chrono::high_resolution_clock::now();
        //for ( auto& path : assets ) {
        //    manager.load(path);
        //}
        //auto endSync = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> syncTime = endSync - startSync;
        //std::cout << "Synchronous load: " << syncTime.count() << "s\n";

        //// --- TEST 2: Async batch load ---
        //auto startAsync = std::chrono::high_resolution_clock::now();
        //auto futures = manager.loadBatchAsync(assets);
        //for ( auto& f : futures ) {
        //    try {
        //        f.get(); // block until done
        //    }
        //    catch ( const std::exception& e ) {
        //        std::cerr << "Error loading: " << e.what() << '\n';
        //    }
        //}
        //auto endAsync = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> asyncTime = endAsync - startAsync;
        //std::cout << "Asynchronous load: " << asyncTime.count() << "s\n";

        //// --- TEST 3: Verify cache safety ---
        //auto againFutures = manager.loadBatchAsync(assets);
        //for ( auto& f : againFutures ) f.get();
        //std::cout << "Cache reload completed safely.\n";

        //// --- Report ---
        //if ( asyncTime.count() < syncTime.count() ) {
        //    std::cout << " Multithreaded loading confirmed faster.\n";
        //}
        //else {
        //    std::cout << " No performance gain (assets may load too fast to notice).\n";
        //}
    }

}
