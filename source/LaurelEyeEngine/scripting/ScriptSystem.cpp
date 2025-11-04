#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"

namespace LaurelEye::Scripting {
    ScriptSystem::ScriptSystem(ScriptSystemType type) {
        systemType = type;
    }

    void ScriptSystem::initialize() {
        switch ( systemType ) {
        case ScriptSystemType::Sol2:
            scriptEngineState = std::make_unique<Sol2State>();
            break;
        }
        scriptEngineState->initialize(context);
    }

    void ScriptSystem::update(float deltaTime) {

        // Make a snapshot of the component pointers so we can iterate safely
        // even if registerComponent() / deregisterComponent() mutates `components`
        // a better bandaid fix, but could become a pain point for 1000's of scripts
        std::vector<ComponentPtr> snapshot;
        snapshot.reserve(components.size());
        snapshot.insert(snapshot.end(), components.begin(), components.end());

        for ( auto* comp : snapshot ) {
            if ( comp->isActive() && comp->getScriptInstance()) {
                comp->getScriptInstance()->onUpdate(deltaTime);
            }
        }
    }

    void ScriptSystem::shutdown() {
        for ( auto* comp : components ) {
            if ( comp->getScriptInstance() ) {
                comp->getScriptInstance()->onShutdown();
            }
        }
        if ( scriptEngineState ) {
            scriptEngineState->shutdown();
            scriptEngineState.reset();
        }
    }

     void ScriptSystem::registerComponent(const ComponentPtr comp) {
        if ( !comp->getScriptPath().empty() ) {
            comp->setScriptInstance(scriptEngineState->createInstance(comp->getScriptPath(), comp->getOwner()));
            if ( comp->getScriptInstance() )
                comp->getScriptInstance()->onStart();
        }
        ISystem<ScriptComponent>::registerComponent(comp);
    }

    void ScriptSystem::deregisterComponent(const ComponentPtr comp) {
        if ( !comp ) return;
        // Clean up Lua instance
        if ( auto* inst = comp->getScriptInstance() ) {
            inst->onShutdown(); // Let the Lua script handle cleanup
            comp->destroyScriptInstance();
        }
        ISystem<ScriptComponent>::deregisterComponent(comp);
    }
} // namespace LaurelEye::Scripting
