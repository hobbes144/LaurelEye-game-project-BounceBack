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
#include "LaurelEyeEngine/transform/TransformComponent.h"

namespace LaurelEye::Audio {
    void SpeakerComponent::playSound(const std::string& soundName) {
        if ( owner ) {
            audioManager->playSound(soundName);
            isPlaying = true;
        }
    }

    void SpeakerComponent::stopSound(const std::string& soundName) {
        audioManager->stopSound(soundName);
        isPlaying = false;
    }

    void SpeakerComponent::pauseSound(const std::string& soundName) {
        audioManager->pauseSound(soundName);
    }

    void SpeakerComponent::resumeSound(const std::string& soundName) {
        audioManager->resumeSound(soundName);
    }

    float SpeakerComponent::getVolume() const {
        return audioManager->getVolume(audioName);
    }

    void SpeakerComponent::setVolume(float vol) {
        audioManager->setVolume(audioName, vol);
    }

    void SpeakerComponent::setAudioManager(IAudioManager* manager) {
        audioManager = manager;
    }

    void SpeakerComponent::createAudioAsset(const std::string& path, float vol, bool is3D, bool loop) {
        audioManager->createSound(audioName, path, vol, is3D, loop);
    }

    void SpeakerComponent::loadAudioAsset() {
        audioManager->loadSound(audioName);
    }

    void SpeakerComponent::loadAudioAssetImmidiate() {
        audioManager->loadSoundImmidiate(audioName);
    }
} // namespace LaurelEye::Audio
