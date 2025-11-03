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

namespace LaurelEye::Particles {

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

    void ParticleEmitterComponent::BindTransform(LaurelEye::TransformComponent* t) {
        boundTransform = t;
    }

    LaurelEye::TransformComponent* ParticleEmitterComponent::GetBoundTransform() const {
        return boundTransform;
    }

} // namespace LaurelEye::Particles
