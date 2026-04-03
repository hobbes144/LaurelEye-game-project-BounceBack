/// @file   AudioSystem.h
/// @author Daoming Wang (KeitherJinn@outlook.com),Martin Chow (
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   11-11-2025
/// @brief  A audio system that manages audio components and interfaces with audio backends.
#pragma once

#include <memory>

#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/audio/MusicSystem.h"
#include "LaurelEyeEngine/audio/UISoundSystem.h"
#include "LaurelEyeEngine/audio/SoundEffectSystem3D.h"
#include "LaurelEyeEngine/audio/SoundEffectSystem2D.h"
#include "LaurelEyeEngine/audio/AudioLoader.h"
#include "LaurelEyeEngine/audio/AudioComponent.h"
#include "LaurelEyeEngine/io/AssetManager.h"

namespace LaurelEye::Audio {
    enum class AudioSystemType {
        FMod
    };

    class AudioSystem : public ISystem<AudioComponent> {
    public:
        explicit AudioSystem(IAudioManager* manager, AudioSystemType type = AudioSystemType::FMod);

        ~AudioSystem();

        void initialize() override;
        void update(float dt) override;
        void shutdown() override;

        // Listener — just forwards to IAudioManager, nothing stored here
        //void setListenerPosition(const Vector3& position);
        //void setListenerVelocity(const Vector3& velocity);

        //MusicSystem& getMusicSystem() { return musicSystem; }
        //UISoundSystem& getUISoundSystem() { return uiSoundSystem; }
        //SoundEffectSystem3D& getSoundEffectSystem3D() { return soundEffectSystem3D; }
        //SoundEffectSystem2D& getSoundEffectSystem2D() { return soundEffectSystem2D; }
        
        
        //  Fmod use [0,1] volume range!!!!!!!!!!!!!!!
        //  --- Global Volume Controls ---
        void setMasterVolume(float volume);
        void setMusicVolume(float volume);
        void setSFXVolume(float volume);
        void setUIVolume(float volume);

        // --- Global Pause / Mute Controls ---
        void pauseMaster(bool paused);
        void pauseMusic(bool paused);
        void pauseSFX(bool paused);
        void pauseUI(bool paused);

        // Optional: global mute toggles
        void muteMaster(bool muted);
        void muteMusic(bool muted);
        void muteSFX(bool muted);
        void muteUI(bool muted);

        // --- Listener Controls ---
        void setListenerPosition(const Vector3& position);
        void setListenerVelocity(const Vector3& velocity);
        void setListenerForward(const Vector3& forward);

        // --- Really Optional I dont care: Global Effects ---
        void fadeOutAll(float speed); // fades all groups
        void fadeInAll(float speed);  // fades all groups

        IAudioManager* getAudioManager() { return audioManager; }
    private:
        void updateComponents(float dt);
        void updateSpatialAudio(AudioComponent& audio);
        void updateSoundEntries(AudioComponent& audio, float dt);

        AudioSystemType systemType;
        IAudioManager* audioManager = nullptr;

        // listener
        Vector3 listenerForward{0.0f, 0.0f, 1.0f};
        Vector3 listenerVelocity{0.0f, 0.0f, 0.0f};
        Vector3 listenerPosition{0.0f, 0.0f, 0.0f};

        // only Channel is enough, not need System unless we want to add more features in the future, like crossfading, or music ducking
        //MusicSystem musicSystem;
        //UISoundSystem uiSoundSystem;
        //SoundEffectSystem3D soundEffectSystem3D;
        //SoundEffectSystem2D soundEffectSystem2D;

        // we dont need this for Fmod sadly
        //AudioLoader audioLoader;
    };
}
