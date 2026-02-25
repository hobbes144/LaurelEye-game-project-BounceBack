#include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
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
#include "LaurelEyeEngine/physics/interfaces/IRigidBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletRigidBody.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/events/CollisionEvent.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/events/IEvent.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/io/Assets.h"
#include <btBulletDynamicsCommon.h>
#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/core/Engine.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/core/ResourceCoordinator.h"
#include "LaurelEyeEngine/core/SystemCoordinator.h"

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
    void renderTestExtended(IWindow* window, GlfwPlatform* glfwP, InputManager* inputManager);
    void eventTest();
    void inputPhysTest(LaurelEye::GlfwPlatform& glfwP, LaurelEye::InputManager& inputManager);
    void scriptingBasicTest();
    void scriptingMultipleTest();
    void scriptingMathTest();
    void scriptingInputTest(GlfwPlatform* glfwP, InputManager* pInputManager);
    void scriptingSceneTest();
    void scriptingECSTest();
    void engineContextServiceTest();
    void engineSystemCoordinatorWiringTest();
    void engineContextOverwriteTest();
    void collisionEventTest();
    void ThreadingTest();
}
