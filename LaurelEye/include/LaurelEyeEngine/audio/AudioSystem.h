/// @file   AudioSystem.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   11-11-2025
/// @brief  A audio system that manages audio components and interfaces with audio backends.
#pragma once

#include <memory>

#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/ecs/ISystem.h"

namespace LaurelEye::Audio {
    /// @brief Defines audio backends for the PhysicsSystem.
    enum class AudioSystemType {
        FMod
    };

    class AudioSystem : public ISystem<SpeakerComponent> {
    public:
        AudioSystem(AudioSystemType type = AudioSystemType::FMod);
        ~AudioSystem();

        void initialize() override;
        void update(float dt) override;
        void shutdown() override;

        // setters and getters for listener properties
        void setListenerPosition(const Vector3& position);
        Vector3 getListenerPosition() const;
        void setListenerVelocity(const Vector3& velocity);
        Vector3 getListenerVelocity() const;
        void updateComponent(SpeakerComponent* comp);

        IAudioManager* getAudioManager() { return audioManager; }

    private:
        AudioSystemType systemType;
        IAudioManager* audioManager = nullptr;
    };
}
