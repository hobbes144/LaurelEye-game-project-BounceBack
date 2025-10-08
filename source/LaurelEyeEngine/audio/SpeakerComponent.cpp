/*!****************************************************************************
 * \file   Speaker.cpp
 * \author Daoming Wang (KeitherJinn@outlook.com)
 * \par    **DigiPen Email**
 *    daoming.wang@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   10-01-2025
 * \brief  Speaker component implementation.
 * 
 *****************************************************************************/

#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/pch.hpp"
#include "LaurelEyeEngine/transform/TransformComponent.h"

namespace LaurelEye::Audio {
    void SpeakerComponent::playSound(const std::string& soundName) {
        TransformComponent* tr = owner->LaurelEye::Entity::findComponent<LaurelEye::TransformComponent>();
        Transform position = tr->getWorldTransform();
        audioManager->playSound(soundName, position.getPosition(), getVolume());
        isPlaying = true;
    }

    void SpeakerComponent::stopSound(const std::string& soundName) {
        audioManager->stopSound(soundName);
        isPlaying = false;
    }

    float SpeakerComponent::getVolume() const {
        return volume;
    }

    float SpeakerComponent::setVolume(float vol) {
        volume = vol;
        return volume;
    }

    void SpeakerComponent::setAudioManager(IAudioManager* manager) {
        audioManager = manager;
    }
} // namespace LaurelEye::Audio
