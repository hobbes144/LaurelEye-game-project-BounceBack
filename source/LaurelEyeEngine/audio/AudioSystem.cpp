/// @file   AudioSystem.cpp
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   11-11-2025
/// @brief  Implementation of AudioSystem that manages audio components and interfaces with audio backends.

#include "LaurelEyeEngine/audio/AudioSystem.h"

namespace LaurelEye::Audio {
    AudioSystem::AudioSystem(AudioSystemType type) {
        systemType = type;
    }

    AudioSystem::~AudioSystem() {
        shutdown();
    }

    void AudioSystem::setListenerPosition(const Vector3& position) {
        audioManager->setListenerPosition(position);
    }

    Vector3 AudioSystem::getListenerPosition() const {
        return audioManager->getListenerPosition();
    }

    void AudioSystem::setListenerVelocity(const Vector3& velocity) {
        audioManager->setListenerVelocity(velocity);
    }

    Vector3 AudioSystem::getListenerVelocity() const {
        return audioManager->getListenerVelocity();
    }

    void AudioSystem::initialize() {
        switch ( systemType ) {
        case AudioSystemType::FMod:
            audioManager = new FModAudioManager();
            break;
            // Add more backends here
        }
    }

    void AudioSystem::update(float dt) {
        for ( size_t i = 0; i < components.size(); i++ ) {
            components[i]->update();
        }
        audioManager->update();
    }

    void AudioSystem::shutdown() {
        audioManager->shutdown();
    }
}
