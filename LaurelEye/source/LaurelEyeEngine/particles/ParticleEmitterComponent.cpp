///
/// @file   ParticleEmitterComponent.cpp
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   10-27-2025
/// @brief
///

#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Particles {

    void ParticleEmitterComponent::Play() {
        isPlaying = true;
        timer = -1.0f;
    }

    void ParticleEmitterComponent::Pause() {
        isPlaying = false;
        timer = -1.0f;
    }

    void ParticleEmitterComponent::Toggle() {
        LE_DEBUG_INFO("Particles", "Particle toggled");
        isPlaying = !isPlaying;
        timer = -1.0;
    }

    void ParticleEmitterComponent::PlayFor(float t) {
        isPlaying = true;
        timer = t;
    }

    void ParticleEmitterComponent::PauseIn(float t) {
        timer = t;
    }

    //--- Getters ---
    ParticleEmitterData& ParticleEmitterComponent::GetEmitterData() {
        return emitterData;
    }

    const ParticleEmitterData& ParticleEmitterComponent::GetEmitterData() const {
        return emitterData;
    }

    std::vector<ParticleData>& ParticleEmitterComponent::GetParticles() {
        return particles;
    }

    const std::vector<ParticleData>& ParticleEmitterComponent::GetParticles() const {
        return particles;
    }

    bool ParticleEmitterComponent::GetIsPlaying() const {
        return isPlaying;
    }

    float ParticleEmitterComponent::GetEmissionAccumulator() const {
        return emissionAccumulator;
    }

    size_t ParticleEmitterComponent::GetMaxParticles() const {
        return maxParticles;
    }

    //--- Setters / Modifiers ---
    void ParticleEmitterComponent::SetMaxParticles(size_t max) {
        maxParticles = max;
        particles.resize(maxParticles);
    }

    void ParticleEmitterComponent::SetEmissionAccumulator(float value) {
        emissionAccumulator = value;
    }

    void ParticleEmitterComponent::IncrementEmittedParticles(float dt) {
        emissionAccumulator += emitterData.emissionRate * dt;
    }

    void ParticleEmitterComponent::DecrementEmissionAccumulator() {
        emissionAccumulator -= 1.0f;
    }

    //--- Utilities ---
    void ParticleEmitterComponent::PopulateParticles() {
        particles.resize(maxParticles);
    }
    void ParticleEmitterComponent::ClearParticles() {
        particles.clear();
        particles.resize(maxParticles);
        emissionAccumulator = 0.0f;
    }
    void ParticleEmitterComponent::UpdateTimer(float dt) {
        if ( timer >= 0.0f ) {
            timer -= dt;
            if ( timer < 0.0f ) {
                Toggle();
            }
        }
    }

    void ParticleEmitterComponent::BindTransform(LaurelEye::TransformComponent* t) {
        boundTransform = t;
    }

    LaurelEye::TransformComponent* ParticleEmitterComponent::GetBoundTransform() const {
        return boundTransform;
    }

} // namespace LaurelEye::Particles
