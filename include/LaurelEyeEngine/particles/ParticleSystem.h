///
/// @file   ParticleSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/27/2025
/// @brief updates and spawns particles (time, position, velocity, color, etc.)
///

#pragma once

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"

namespace LaurelEye::Particles {

    class ParticleSystem : public ISystem<ParticleEmitterComponent> {
    public:
        ParticleSystem() = default;

        void initialize() override;
        void update(float deltaTime) override;
        void shutdown() override;

        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(const ComponentPtr component) override;

        void setMaxParticles(unsigned int);

    private:
        unsigned int activeParticles = 0;

        unsigned int MaxParticles = 1000;

        std::shared_ptr<Graphics::ParticleRenderPass> prp;
        std::shared_ptr<Graphics::GeometryBuffer> particleBuffer;

        void spawnParticle(ParticleEmitterComponent& emitter);
        LaurelEye::Vector3 randomDirection(ParticleEmitterComponent& emitter);

        void updateParticleBufferObject();
        void dispatchNumParticlesToRenderPass();

    };

}
