#pragma once

#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/IComponent.h"

namespace LaurelEye::Audio {
    class SpeakerComponent : public IComponent {
    public:
        SpeakerComponent(IAudioManager* am = nullptr, float vol = 1.0f): volume(1.0f), isPlaying(false), audioManager(am) {}
        ~SpeakerComponent() override = default;
        void playSound(const std::string& soundName);
        void stopSound(const std::string& soundName);
        float getVolume() const;
        float setVolume(float vol);

        void setAudioManager(IAudioManager* manager);

    private:
        float volume;
        bool isPlaying;

        IAudioManager* audioManager; // Pointer to the AudioManager instance
    };
} // namespace LaurelEye::Audio
