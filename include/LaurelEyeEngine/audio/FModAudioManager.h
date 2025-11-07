/*!****************************************************************************
 * \file   AudioManager.h
 * \author Daoming Wang (KeitherJinn@outlook.com)
 * \par    **DigiPen Email**
 *    daoming.wang@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   10-01-2025
 * \brief  Audio class using FMOD to implement all audio functionality.
 *
 *****************************************************************************/

#pragma once
#ifndef FMODAUDIOMANAGER_H
#define FMODAUDIOMANAGER_H

#include "LaurelEyeEngine/audio/AudioAsset.h"
#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include <fmod.hpp>
#include <unordered_map>
#include <vector>

namespace LaurelEye::Audio {
    /*!****************************************************************************
     * \brief Audio Manager
     *
     * ## Usage:
     *
     * The AudioManager manages all audio functionality using FMOD. It is responsible
     * for initializing the audio system, loading sounds, playing sounds, updating
     * audio each frame, and properly shutting down the audio system.
     *
     * ## Pre-initialization calls:
     *
     * N/A
     *
     * (AudioManager is created on first use; be sure to call init() before any
     * other audio operations.)
     *
     * ## General lifecycle of an AudioManager:
     *
     * - Start the AudioManager instance using AudioManager::instance().
     * - Call init() once at startup to initialize the audio system:
     *   - Example: AudioManager::instance().init();
     * - Load all necessary sound assets using loadSound():
     *   - Example: AudioManager::instance().loadSound("pew", "/media/pew.mp3", true, false);
     *     - The third parameter indicates whether the sound is 3D (true) or 2D (false).
     *     - Optionally, a loop flag can be provided on the 4th parameter.
     * - Play sounds using playSound():
     *   - Example: AudioManager::instance().playSound("pew", 0.0f, 0.0f, 0.0f);
     * - Stop a sound using stopSound():
     *   - Example: AudioManager::instance().stopSound("radio");
     * - In the main loop, call update() every frame to process audio updates:
     *   - Example: AudioManager::instance().update();
     *   - You need to call update every frame to play the sounds properly.
     * - Adjust the listener position using setListenerPosition() to control 3D audio:
     *   - Example: AudioManager::instance().setListenerPosition(x, y, z);
     * - To change playback speed, call setPlaybackSpeed():
     *   - Example: AudioManager::instance().setPlaybackSpeed(0.5f);
     * - To toggle playback speed, call togglePlaybackSpeed(), when called again reset the speed.
     *   - Example: AudioManager::instance().togglePlaybackSpeed(0.7f);
     * - Before terminating the application, call shutdown() to clean up the audio system:
     *   - Example: AudioManager::instance().shutdown();
     *
     * ## Defaults:
     *
     * - Maximum channels: 512 (can be adjusted via the init() parameter)
     * - Sounds are non-looping unless the loop flag is set during loadSound()
     *
     * ## Notes:
     *
     * - You need to call update() every frame to ensure proper audio processing.
     * - It is recommended to load all required sounds during initialization to avoid
     *   runtime performance issues.
     *
     *****************************************************************************/
    class FModAudioManager : public IAudioManager {
    public:
        FModAudioManager() {
            initialize();
        }

        FModAudioManager(int maxChanels) {
            initialize(maxChanels);
        }

        ~FModAudioManager() override;

        void initialize(int maxChannels = 512);
        void update() override;
        void shutdown() override;

        void loadSound(const std::string& name, const std::string& path, bool is3D = false, bool loop = false) override;
        void playSound(const std::string& name, const Vector3& position = Vector3(), float volume = 1.0f) override;
        void playMovingSound(const std::string& name, const Vector3& position = Vector3(), const Vector3& velocity = Vector3(), float volume = 1.0f) override;
        void stopSound(const std::string& name) override;
        void setListenerPosition(const Vector3& position) override;
        void setPlaybackSpeed(float speed) override;
        void togglePlaybackSpeed(float speed = 1) override;
        void setVolume(const std::string& name, float volume) override;
        void setMasterVolume(float volume) override;

        void pauseSound(const std::string& name) override;
        void resumeSound(const std::string& name) override;
        void pauseAllSound() override;
        void resumeAllSound() override;

        std::string readAudioConfig(const std::string& configFilePath) override;

        Vector3 getListenerPosition() const override { return listenerPosition; }

    private:
        FMOD::System* fmodSystem_ = nullptr;
        std::unordered_map<std::string, AudioAsset*> sounds_;

        float currentPlaybackSpeed_ = 1.0f;
        Vector3 listenerPosition = Vector3();

        std::vector<AudioAsset*> awatingSounds;
    };

#endif // AUDIO_H

} // namespace LaurelEye::Audio
