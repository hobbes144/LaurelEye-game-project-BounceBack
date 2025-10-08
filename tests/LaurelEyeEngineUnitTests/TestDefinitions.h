#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/platforms/glfw/GlfwPlatform.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine//input/IInput.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/memory/MemoryManager.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/physics/interfaces/IBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletBody.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include <iostream>
#include <cassert>
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <btBulletDynamicsCommon.h>
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
    void inputTest();
    void transformTest();
    void transformSystemTest();
    void transformHierarchyTest();
    void audioPlayTest();
    void audioStopTest();
    void audioPauseTest();
    void audioResumeTest();
    void speakerTest();
    void speakerTest2();
    void speakerTest3();
    void memoryTest();
    void assetMeshTest();
    void assetImageTest();
    void assetCacheTest();
    void physicsComponentTest();
    void inputPhysTest(LaurelEye::GlfwPlatform& glfwP, LaurelEye::InputManager& inputManager);
}
