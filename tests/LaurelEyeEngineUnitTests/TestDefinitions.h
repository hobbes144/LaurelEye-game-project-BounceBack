#include "LaurelEyeEngine/input/InputSystem.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/transform/TransformSystem.h"
#include <iostream>
#include <cassert>
#include <cassert>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace LaurelEye {
    void mathTest();
    void windowTest();
    void entityTest();
    void sceneTest();
    void physicsTest();
    void transformTest();
    void transformSystemTest();
    void transformHierarchyTest();
}
