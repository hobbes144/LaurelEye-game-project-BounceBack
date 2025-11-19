///
/// @file   Sol2API_Particles.cpp
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-17-2025
/// @brief  
///

#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Particles.h"

namespace LaurelEye::Scripting {

    void Sol2API_Particles::setup(sol::state& lua, EngineContext* ctx) {

        //ParticleEmitterData
        lua.new_usertype<Particles::ParticleEmitterData>(
            "ParticleEmitterData",
            //Constructors
            sol::constructors<Particles::ParticleEmitterData()>(),
            //Emission Properties
            "emissionRate", &Particles::ParticleEmitterData::emissionRate,
            "spreadAngle", &Particles::ParticleEmitterData::spreadAngle,
            "particleLifetime", &Particles::ParticleEmitterData::particleLifetime,
            "lifetimeVariation", &Particles::ParticleEmitterData::lifetimeVariation,
            "initialSpeed", &Particles::ParticleEmitterData::initialSpeed,
            "speedVariation", &Particles::ParticleEmitterData::speedVariation,
            "startSize", &Particles::ParticleEmitterData::startSize,
            "endSize", &Particles::ParticleEmitterData::endSize,
            "sizeVariation", &Particles::ParticleEmitterData::sizeVariation,
            "startColor", &Particles::ParticleEmitterData::startColor,
            "endColor", &Particles::ParticleEmitterData::endColor,
            "usePhysics", &Particles::ParticleEmitterData::usePhysics
        );

        //ParticleEmitterComponent
        lua.new_usertype<Particles::ParticleEmitterComponent>(
            "ParticleEmitterComponent",
            sol::no_constructor,
            //Emission Control
            "play", &Particles::ParticleEmitterComponent::Play,
            "pause", &Particles::ParticleEmitterComponent::Pause,
            "toggle", &Particles::ParticleEmitterComponent::Toggle,
            "playFor", &Particles::ParticleEmitterComponent::PlayFor,
            "pauseIn", &Particles::ParticleEmitterComponent::PauseIn,
            //Queries
            "isPlaying", &Particles::ParticleEmitterComponent::GetIsPlaying,
            //"getEmitterData", &Particles::ParticleEmitterComponent::GetEmitterData,
            "getMaxParticles", &Particles::ParticleEmitterComponent::GetMaxParticles,
            "getEmissionAccumulator", &Particles::ParticleEmitterComponent::GetEmissionAccumulator,
            "setMaxParticles", &Particles::ParticleEmitterComponent::SetMaxParticles
        );

    }

}
