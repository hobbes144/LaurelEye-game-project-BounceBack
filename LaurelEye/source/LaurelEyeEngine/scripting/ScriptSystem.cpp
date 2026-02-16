#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptBroker.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"

namespace LaurelEye::Scripting {
    ScriptSystem::ScriptSystem(ScriptSystemType type) {
        systemType = type;
    }

    void ScriptSystem::initialize() {
        switch ( systemType ) {
        case ScriptSystemType::Sol2:
            scriptEngineState = std::make_unique<Sol2State>();
            broker = std::make_unique<Sol2ScriptBroker>();
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
            if ( comp->isActive() && comp->getScriptInstance() ) {
                comp->getScriptInstance()->onUpdate(deltaTime);
            }
        }
        broker->flushMessageQueue();
    }

    void ScriptSystem::shutdown() {
        broker->clearMessageQueue();
        for ( auto* comp : components ) {
            if ( comp->getScriptInstance() ) {
                comp->getScriptInstance()->onShutdown();
                comp->destroyScriptInstance();
            }
        }
        if ( scriptEngineState ) {
            scriptEngineState->shutdown(context);
            scriptEngineState.reset();
        }
    }

    void ScriptSystem::registerComponent(const ComponentPtr comp) {
        if ( !comp->getScriptPath().empty() ) {
            comp->setScriptInstance(scriptEngineState->createInstance(comp->getScriptPath(), comp->getOwner()));
            if ( comp->getScriptInstance() ) {
                broker->registerScript(comp->getOwner(), comp->getScriptInstance());
                comp->getScriptInstance()->onStart();
            }
        }
        ISystem<ScriptComponent>::registerComponent(comp);
    }

    void ScriptSystem::deregisterComponent(const ComponentPtr comp) {
        if ( !comp ) return;
        // Clean up Lua instance
        if ( auto* inst = comp->getScriptInstance() ) {
            broker->deregisterScript(comp->getOwner());
            inst->onShutdown(); // Let the Lua script handle cleanup
            comp->destroyScriptInstance();
        }
        ISystem<ScriptComponent>::deregisterComponent(comp);
    }
} // namespace LaurelEye::Scripting
