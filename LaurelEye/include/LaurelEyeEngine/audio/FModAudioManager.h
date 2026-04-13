/*!****************************************************************************
 * \file   FModAudioManager.h
 * \author Daoming Wang (KeitherJinn@outlook.com)
 * \par    **DigiPen Email**
 *    daoming.wang@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   10-01-2025
 * \brief  Audio class using FMOD to implement all audio functionality with AudioAsset class.
 *
 *****************************************************************************/

#pragma once
#ifndef FMODAUDIOMANAGER_H
#define FMODAUDIOMANAGER_H

#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include <fmod.hpp>
#include <unordered_map>

namespace LaurelEye::Audio {

    class FModAudioManager : public IAudioManager {
    public:
        explicit FModAudioManager(int maxChannels = 512);
        ~FModAudioManager() override;

        // --- Lifecycle ---
        void initialize() override;
        void update() override;
        void shutdown() override;

        // --- Sound creation ---
        SoundHandle createSound(const std::string& path,
                                bool is3D,
                                bool loop) override;

        SoundHandle createSoundAsync(const std::string& path,
                                     bool is3D,
                                     bool loop) override;

        SoundLoadState getSoundLoadState(SoundHandle sound) override;

        void destroySound(SoundHandle sound) override;

        // --- Playback ---
        ChannelHandle playSound(SoundHandle sound,
                                float volume = 1.0f,
                                SoundCategory category = SoundCategory::SFX) override;

        // fire and forget 3D sound at the given position and velocity, without needing to manage the channel
        // we not using it for the current Fmod design, but if we use for another sound lib may need it
        void playOneShot3D(SoundHandle sound,
                           const Vector3& pos,
                           const Vector3& vel,
                           float volume = 1.0f,
                           SoundCategory category = SoundCategory::SFX) override;

        void stopChannel(ChannelHandle channel) override;
        void pauseChannel(ChannelHandle channel,
                          bool paused) override;
        void setChannelVolume(ChannelHandle channel,
                              float volume) override;
        bool isChannelPlaying(ChannelHandle channel) override;

        // --- 3D attributes ---
        void setChannel3DAttributes(ChannelHandle channel,
                                    const Vector3& position,
                                    const Vector3& velocity) override;

        // --- Listener ---
        void setListenerAttributes(const Vector3& position,
                                   const Vector3& velocity,
                                   const Vector3& forward) override;

        void setGroupVolume(ChannelGroupHandle group,
                            float volume) override;
        void pauseGroup(ChannelGroupHandle group,
                        bool paused) override;

        // Channel groups
        ChannelGroupHandle getMusicGroup() override { return musicGroupHandle; }
        ChannelGroupHandle getSFXGroup() override { return sfxGroupHandle; }
        ChannelGroupHandle getUIGroup() override { return uiGroupHandle; }

        // Raw FMOD access (for systems)
        FMOD::Channel* getRawChannel(ChannelHandle handle);

        float getGroupVolume(ChannelGroupHandle group) override;

    private:
        FMOD::System* system = nullptr;

        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD::ChannelGroup* musicGroup = nullptr;
        FMOD::ChannelGroup* sfxGroup = nullptr;
        FMOD::ChannelGroup* uiGroup = nullptr;

        ChannelGroupHandle masterGroupHandle = NULL_GROUP;
        ChannelGroupHandle musicGroupHandle = NULL_GROUP;
        ChannelGroupHandle sfxGroupHandle = NULL_GROUP;
        ChannelGroupHandle uiGroupHandle = NULL_GROUP;

        int maxChannels = 512;
        uint32_t nextSoundHandle = 1;   // 0 is reserved for NULL_SOUND
        uint32_t nextChannelHandle = 1; // 0 is reserved for NULL_CHANNEL
        uint32_t nextGroupHandle = 1;   // 0 reserved for NULL_GROUP

        std::unordered_map<SoundHandle, FMOD::Sound*> sounds;
        std::unordered_map<ChannelHandle, FMOD::Channel*> channels;
        std::unordered_map<ChannelGroupHandle, FMOD::ChannelGroup*> groups;

        // Helpers
        void createDefaultGroups();
        ChannelGroupHandle registerGroup(FMOD::ChannelGroup* group);
        FMOD::ChannelGroup* getGroup(ChannelGroupHandle handle);

        // Helper: silently remove stale channels FMOD has already freed
        void pruneFinishedChannels();
    };


} // namespace LaurelEye::Audio
#endif // AUDIO_H

