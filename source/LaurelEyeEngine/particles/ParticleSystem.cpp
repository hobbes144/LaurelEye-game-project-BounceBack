///
/// @file   PhysicsSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/27/2025
/// @brief ECS-integrated particle system.
/// manages simulation lifecycle, features of the particles.
///

#include "LaurelEyeEngine/particles/ParticleSystem.h"

// #include <cmath>

namespace LaurelEye::Particles {

    void ParticleSystem::initialize() {
        // if ( !particleBuffer ) {
        //
        //     Graphics::GeometryBuffer::ModifiableAttributes tempAttributes;
        //
        //     tempAttributes[Graphics::GeometryBuffer::AttributeType::Position] = {
        //         std::vector<float>(500*3), // data
        //         3,                       // elementSize (x, y, z)
        //         GL_FLOAT,
        //         GL_FALSE,
        //         0 // stride handled internally if interleaved
        //     };
        //
        //     tempAttributes[LaurelEye::Graphics::GeometryBuffer::AttributeType::Color] = {
        //         std::vector<float>(500*4), // data
        //         4,                       // elementSize (r, g, b, a)
        //         GL_FLOAT,
        //         GL_FALSE,
        //         0};
        //
        //     particleBuffer = Graphics::GeometryBuffer::create(
        //         tempAttributes,
        //         "ParticleBuffer",
        //         false // interleaved
        //     );
        // }

        prp = context->getService<Graphics::RenderSystem>()->retrieveParticlePass();
        // prp->updateParticleBuffer(particleBuffer);
    }

    void ParticleSystem::update(float dt) {

        //---For Each Emitter Component---
        for ( auto* emitter : components ) {

            emitter->IncrementEmittedParticles(dt);

            //---Spawn New Particles---
            // While the Emitter is still poised to emit
            while ( emitter->GetEmissionAccumulator() >= 1.0f ) {
                emitter->DecrementEmissionAccumulator();
                // Spawn a new Particle
                spawnParticle(*emitter);
            }

            //---Update Particles---
            // For every particle in the emitter
            for ( auto& p : emitter->GetParticles() ) {
                if ( !p.active ) continue;

                // Advance Lifetime
                p.lifetime += dt;
                // Deactivate if Aged out
                if ( p.lifetime > p.maxLifetime ) {
                    p.active = false;
                    --activeParticles;
                    continue;
                }

                // Update Pos (NO PHYSICS)
                p.position += p.velocity * dt;

                // Interpolate Color / Size
                float t = p.lifetime / p.maxLifetime;
                for ( int i = 0; i < 4; ++i )
                    p.color[i] = emitter->GetEmitterData().startColor[i] * (1 - t) + emitter->GetEmitterData().endColor[i] * t;
                p.size = emitter->GetEmitterData().startSize * (1 - t) + emitter->GetEmitterData().endSize * t;
            }
        }

        updateParticleBufferObject();
        dispatchNumParticlesToRenderPass();
    }

    void ParticleSystem::shutdown() {
        for ( auto* emitter : components )
            emitter->ClearParticles();
    }

    void ParticleSystem::registerComponent(const ComponentPtr component) {
        // Call base
        LaurelEye::ISystem<ParticleEmitterComponent>::registerComponent(component);

        // Configure New Emitter Components Properly
        component->ClearParticles();
        component->PopulateParticles();
        component->rng.seed(std::random_device{}());
    }

    void ParticleSystem::spawnParticle(ParticleEmitterComponent& emitter) {
        for ( auto& p : emitter.GetParticles() ) {
            if ( !p.active ) {
                ++activeParticles;
                p.active = true;
                p.lifetime = 0.0f;

                const auto& data = emitter.GetEmitterData();
                auto rand = [&](float base, float variance) {
                    return base + emitter.randomSpread(emitter.rng) * variance;
                };

                p.maxLifetime = std::max(0.05f, rand(data.particleLifetime, data.lifetimeVariation));
                p.position = data.position;
                p.velocity = randomDirection(emitter) * rand(data.initialSpeed, data.speedVariation);
                p.size = rand(data.startSize, data.sizeVariation);
                p.rotation = emitter.randomSpread(emitter.rng) * 3.14159f;

                // Slight random tint
                p.color = data.startColor;
                for ( int i = 0; i < 3; ++i )
                    p.color[i] *= 1.0f + 0.1f * emitter.randomSpread(emitter.rng);

                return;
            }
        }
    }

    void ParticleSystem::updateParticleBufferObject() {

        if ( components.empty() ) {
            // No emitters → nothing to draw, avoid invalid buffer ops
            return;
        }

        // --- Create GPU buffer if it doesn't exist ---
        if ( !particleBuffer ) {

            Graphics::GeometryBuffer::ModifiableAttributes tempAttributes;

            tempAttributes[Graphics::GeometryBuffer::AttributeType::Position] = {
                std::vector<float>(500 * 3), // data
                3,                           // elementSize (x, y, z)
                GL_FLOAT,
                GL_FALSE,
                0 // stride handled internally if interleaved
            };

            tempAttributes[LaurelEye::Graphics::GeometryBuffer::AttributeType::Color] = {
                std::vector<float>(500 * 4), // data
                4,                           // elementSize (r, g, b, a)
                GL_FLOAT,
                GL_FALSE,
                0};

            particleBuffer = Graphics::GeometryBuffer::create(
                tempAttributes,
                "ParticleBuffer",
                false // interleaved
            );
            prp->updateParticleBuffer(particleBuffer);
        }

        bool hasActiveParticles = false;
        for ( auto* emitter : components ) {
            for ( const auto& p : emitter->GetParticles() ) {
                if ( p.active ) {
                    hasActiveParticles = true;
                    break;
                }
            }
            if ( hasActiveParticles )
                break;
        }

        if ( !hasActiveParticles )
            return; // nothing to upload

        Graphics::GeometryBuffer::ModifiableAttributes attributes;

        attributes[Graphics::GeometryBuffer::AttributeType::Position] = {
            {}, // data
            3,  // elementSize (x, y, z)
            GL_FLOAT,
            GL_FALSE,
            0 // stride handled internally if interleaved
        };

        attributes[LaurelEye::Graphics::GeometryBuffer::AttributeType::Color] = {
            {}, // data
            4,  // elementSize (r, g, b, a)
            GL_FLOAT,
            GL_FALSE,
            0};

        for ( auto* emitter : components ) {
            for ( const auto& p : emitter->GetParticles() ) {
                if ( !p.active )
                    continue;

                // Position
                attributes[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(p.position.x);
                attributes[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(p.position.y);
                attributes[Graphics::GeometryBuffer::AttributeType::Position].data.push_back(p.position.z);

                // Color
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(p.color[0]);
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(p.color[1]);
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(p.color[2]);
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data.push_back(p.color[3]);
            }
        }

        if ( particleBuffer->hasAttribute(Graphics::GeometryBuffer::AttributeType::Position) ) {
            particleBuffer->updateVertexAttribute(
                Graphics::GeometryBuffer::AttributeType::Position,
                attributes[Graphics::GeometryBuffer::AttributeType::Position].data);
        }
        if ( particleBuffer->hasAttribute(Graphics::GeometryBuffer::AttributeType::Color) ) {
            particleBuffer->updateVertexAttribute(
                Graphics::GeometryBuffer::AttributeType::Color,
                attributes[Graphics::GeometryBuffer::AttributeType::Color].data);
        }
    }

    void ParticleSystem::dispatchNumParticlesToRenderPass() {

        prp->updateNumParticles(activeParticles);
    }

    LaurelEye::Vector3 ParticleSystem::randomDirection(ParticleEmitterComponent& emitter) {
        float angle = emitter.GetEmitterData().spreadAngle * (3.14159f / 180.0f);
        float theta = emitter.randomSpread(emitter.rng) * angle;
        float phi = emitter.randomSpread(emitter.rng) * 3.14159f; // random around Y axis
        float x = std::sin(theta) * std::cos(phi);
        float y = std::cos(theta);
        float z = std::sin(theta) * std::sin(phi);
        return LaurelEye::Vector3{x, y, z}.normalized();
    }

} // namespace LaurelEye::Particles
