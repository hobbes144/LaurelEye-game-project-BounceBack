#include "LaurelEyeEngine/core/Engine.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

int main() {
    LaurelEye::EngineConfig config;
    config.projectName = "SampleGame";
    config.assetRoot = "games/sample/assets/";
    config.initialSceneList = "games/sample/scene_list.json";
    config.enableDebugMode = true;

    LaurelEye::Log::config().logEnabledSystems.insert("Scripting");

    LaurelEye::Engine engine(config);
    engine.run();
    return 0;
}
