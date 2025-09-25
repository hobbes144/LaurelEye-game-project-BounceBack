#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"

#include "GLFW/glfw3.h"

namespace LaurelEye {

    void GlfwPlatform::initialize() {
        glfwInit();
    }

    void GlfwPlatform::update() {
        glfwPollEvents();
    }

    void GlfwPlatform::shutdown() {
        glfwTerminate();
    }

} // namespace LaurelEye
