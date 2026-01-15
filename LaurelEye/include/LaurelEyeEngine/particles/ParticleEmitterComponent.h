///
/// @file   ParticleEmitterComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   10-27-2025
/// @brief
///

#pragma once

#include <vector>
#include <random>

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::Particles {

    struct ParticleData {
        LaurelEye::Vector3 position;
        LaurelEye::Vector3 velocity;
        LaurelEye::Vector4 color;
        float size;
        float rotation;
        float lifetime; // current age
        float maxLifetime;
        bool active;
    };

    struct ParticleEmitterData {
        float emissionRate = 50.0f;
        float spreadAngle = 30.0f;

        float particleLifetime = 1.0f;
        float lifetimeVariation = 0.2f;

        float initialSpeed = 5.0f;
        float speedVariation = 1.0f;

        float startSize = 0.2f;
        float endSize = 0.0f;
        float sizeVariation = 0.05f;

        LaurelEye::Vector4 startColor = LaurelEye::Vector4(1.0f, 0.5f, 0.0f, 1.0f);
        LaurelEye::Vector4 endColor = LaurelEye::Vector4(1.0f, 0.0f, 0.0f, 0.0f);
        bool usePhysics = false;
    };


    class ParticleEmitterComponent : public IComponent {

    public:

        //--- Interface ---
        /// @brief Begins Emission of Particles
        void Play();
        /// @brief Ends Emission of Particles
        void Pause();
        /// @brief Toggles Emission of Particles
        void Toggle();
        /// @brief Will Play (or continue Playing) for @tparam seconds
        /// @param t time in seconds
        void PlayFor(float t);
        /// @brief Will Pause in @tparam seconds (Will not begin emission if 
        /// already paused)
        /// @param t time in seconds
        void PauseIn(float t);

        //--- Getters ---
        ParticleEmitterData& GetEmitterData();
        const ParticleEmitterData& GetEmitterData() const;

        std::vector<ParticleData>& GetParticles();
        const std::vector<ParticleData>& GetParticles() const;

        bool GetIsPlaying() const;
        float GetEmissionAccumulator() const;
        size_t GetMaxParticles() const;

        //--- Setters / Modifiers ---
        void SetMaxParticles(size_t max);
        void SetEmissionAccumulator(float value);
        void IncrementEmittedParticles(float dt); // accumulate emission based on rate and dt
        void DecrementEmissionAccumulator();      // decrease accumulator by 1 per emitted particle

        //--- Utilities ---
        void PopulateParticles();
        void ClearParticles();
        void UpdateTimer(float dt);

        void BindTransform(LaurelEye::TransformComponent* t);
        LaurelEye::TransformComponent* GetBoundTransform() const;

        std::default_random_engine rng;
        std::uniform_real_distribution<float> randomSpread{-1.0f, 1.0f};

    private:
        bool isPlaying = true;
        float timer = -1.0f;

        ParticleEmitterData emitterData;
        size_t maxParticles = 500;

        TransformComponent* boundTransform = nullptr;

        std::vector<ParticleData> particles;
        float emissionAccumulator = 0.0f;

    };

} // namespace LaurelEye::Particle
