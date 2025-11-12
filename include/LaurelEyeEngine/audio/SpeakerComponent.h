/// @file   SpeakerComponent.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   11-01-2025
/// @brief  A speaker component that can be attached to entities to play audio according to it's postion, velocity and so on. Also cam play 2D audio.
#pragma once

#include "LaurelEyeEngine/audio/AudioAsset.h"
#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/IComponent.h"

namespace LaurelEye::Audio {
    class SpeakerComponent : public IComponent {
    public:
        SpeakerComponent(IAudioManager* am = nullptr, std::string _audioName = "default")
            : audioName(_audioName), isPlaying(false), playOnLoad(false), audioManager(am) {}
        ~SpeakerComponent() override = default;

        void playSound(const std::string& soundName);
        void playSound() { playSound(audioName); }
        void stopSound(const std::string& soundName);
        void stopSound() { stopSound(audioName); }
        void pauseSound(const std::string& soundName);
        void pauseSound() { pauseSound(audioName); }
        void resumeSound(const std::string& soundName);
        void resumeSound() { resumeSound(audioName); }
        float getVolume() const;
        void setVolume(float vol);

        void setAudioManager(IAudioManager* manager);

        void setAudioName(std::string newAudioName) { audioName = newAudioName; }
        const std::string& getAudioName() { return audioName; }

        void setPlayOnLoad(bool play) { playOnLoad = play; }
        const bool getPlayOnLoad() const { return playOnLoad; }

        // Create audio asset in the audio manager
        void createAudioAsset(const std::string& path, float volume = 1.0f, bool is3D = false, bool loop = true);

        // Load audio asset in the audio manager, only when used after created is valid
        void loadAudioAsset();
        void loadAudioAssetImmidiate();

        // Update speaker component each frame, automatically called by AudioSystem, update sound position, volume and so on.
        void update();

    private:
        bool isPlaying;
        bool playOnLoad;
        std::string audioName;

        IAudioManager* audioManager; // Pointer to the AudioManager instance
    };
} // namespace LaurelEye::Audio
