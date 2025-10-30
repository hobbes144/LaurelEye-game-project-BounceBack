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
        for ( auto* comp : components ) {
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

    void ScriptSystem::registerComponent(const ComponentPtr comp){
        if ( !comp->getScriptPath().empty() ) {
            comp->setScriptInstance(scriptEngineState->createInstance(comp->getScriptPath(), comp->getOwner()));
            if ( comp->getScriptInstance() )
                comp->getScriptInstance()->onStart();
        }
        components.push_back(comp);
    }
 
} // namespace LaurelEye::Scripting
