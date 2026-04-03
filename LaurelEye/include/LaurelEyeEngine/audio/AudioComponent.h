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
#include <string>
#include <unordered_map>

namespace LaurelEye::Audio {

    enum class SoundType {
        SFX2D,
        SFX3D,
        BGM,
        UI
    };

    struct SoundEntry {
        std::string path;                           // key into AssetManager
        ChannelHandle channelHandle = NULL_CHANNEL; // runtime only
        SoundType type = SoundType::SFX2D;
        float volume = 1.0f;
        float fadeSpeed = 1.0f;
        float currentVolume = 0.0f;
        bool fadingIn = false;
        bool fadingOut = false;
        bool looping = false;
        bool active = false;
    };

    class AudioComponent : public IComponent {
    public:
        // Just register intent — no handles, no FMOD
        AudioComponent(IAudioManager* am = nullptr)
            : audioManager(am) {}
        ~AudioComponent() override
        {
            // Stop all channels associated with this component
            for ( auto& [id, entry] : sounds ) {
                if ( entry.channelHandle != NULL_CHANNEL ) {
                    audioManager->stopChannel(entry.channelHandle);
                }
            }
        }
        ;

        void addSound(const std::string& id,
                      const std::string& path,
                      SoundType type,
                      float volume = 1.0f,
                      float fadeSpeed = 1.0f,
                      bool looping = false);

        void removeSound(const std::string& id);

        // Playback — AudioSystem calls these, passing resolved handle
        void play(const std::string& id);
        void stop(const std::string& id);
        void stopAll();

        void setPosition(const Vector3& p) {
            position = p;
        }
        void setVelocity(const Vector3& v) { velocity = v; }

        Vector3 getPosition() const { return position; }
        Vector3 getVelocity() const { return velocity; }

        // AudioSystem reads this to know what to resolve
        std::unordered_map<std::string, SoundEntry>& getSounds() { return sounds; }
        const std::unordered_map<std::string, SoundEntry>& getSounds() const { return sounds; }

    private:
        std::unordered_map<std::string, SoundEntry> sounds;
        Vector3 position{};
        Vector3 velocity{};
        IAudioManager* audioManager;
    };

} // namespace LaurelEye::Audio
