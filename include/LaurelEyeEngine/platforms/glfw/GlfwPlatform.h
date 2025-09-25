#ifdef PLATFORM_GLFW
#pragma once

#include "LaurelEyeEngine/platforms/IPlatform.h"

namespace LaurelEye {

    class GlfwPlatform final : public IPlatform {
    public:
        void initialize() override;
        void update() override;
        void shutdown() override;
    };

} // namespace LaurelEye

#endif
