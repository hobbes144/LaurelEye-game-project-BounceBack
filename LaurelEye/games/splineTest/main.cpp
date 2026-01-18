#include "LaurelEyeEngine/core/Engine.h"

int main() {
    LaurelEye::EngineConfig config;
    config.projectName = "splineTest";
    config.assetRoot = "games/splineTest/assets/";
    config.initialSceneList = "games/splineTest/scene_list.json";
    config.enableDebugMode = true;

    LaurelEye::Engine engine(config);
    engine.run();
    return 0;
}
