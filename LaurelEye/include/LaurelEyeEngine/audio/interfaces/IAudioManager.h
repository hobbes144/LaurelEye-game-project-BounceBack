/*!****************************************************************************
 * \file   IAudioManager.h
 * \author Daoming Wang (KeitherJinn@outlook.com)
 * \par    **DigiPen Email**
 *    daoming.wang@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   10-01-2025
 * \brief  AudioManager interface declaration.
 *
 *****************************************************************************/

#pragma once

#pragma once
#include "LaurelEyeEngine/math/Vector3.h"
#include <string>

namespace LaurelEye::Audio {

    enum class SoundCategory {
        Music,
        SFX,
        UI
    };

    enum class SoundLoadState {
        Loading,
        Ready,
        Failed
    };

    using SoundHandle = uint32_t;
    using ChannelHandle = uint32_t;
    using ChannelGroupHandle = uint32_t;

    inline constexpr SoundHandle NULL_SOUND = 0;
    inline constexpr ChannelHandle NULL_CHANNEL = 0;
    inline constexpr ChannelGroupHandle NULL_GROUP = 0;

    class IAudioManager {
    public:
        virtual ~IAudioManager() = default;

        // --- Lifecycle ---
        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void shutdown() = 0;

        // --- Sound creation ---
        // Returns NULL_SOUND on failure
        virtual SoundHandle createSound(const std::string& path,
                                        bool is3D,
                                        bool loop) = 0;

        virtual SoundHandle createSoundAsync(const std::string& path,
                                             bool is3D,
                                             bool loop) = 0;
        virtual SoundLoadState getSoundLoadState(SoundHandle sound) = 0;

        virtual void destroySound(SoundHandle sound) = 0;

        // --- Playback ---
        // Returns NULL_CHANNEL on failure
        virtual ChannelHandle playSound(SoundHandle sound,
                                        float volume = 1.0f,SoundCategory category = SoundCategory::SFX) = 0;

        // fire and forget 3D sound at the given position and velocity, without needing to manage the channel
        virtual void playOneShot3D(SoundHandle sound,
                                   const Vector3& pos,
                                   const Vector3& vel,
                                   float volume = 1.0f,
                                   SoundCategory category = SoundCategory::SFX) = 0;

        virtual void stopChannel(ChannelHandle channel) = 0;
        virtual void pauseChannel(ChannelHandle channel,
                                  bool paused) = 0;
        virtual void setChannelVolume(ChannelHandle channel,
                                      float volume) = 0;
        virtual bool isChannelPlaying(ChannelHandle channel) = 0;

        // --- 3D attributes ---
        virtual void setChannel3DAttributes(ChannelHandle channel,
                                            const Vector3& position,
                                            const Vector3& velocity) = 0;

        // --- Listener ---
        virtual void setListenerAttributes(const Vector3& position,
                                           const Vector3& velocity,
                                           const Vector3& forward) = 0;

        virtual ChannelGroupHandle getMusicGroup() = 0;
        virtual ChannelGroupHandle getSFXGroup() = 0;
        virtual ChannelGroupHandle getUIGroup() = 0;

        virtual void setGroupVolume(ChannelGroupHandle group, float volume) = 0;
        virtual void pauseGroup(ChannelGroupHandle group, bool paused) = 0;

        virtual float getGroupVolume(ChannelGroupHandle group) = 0;

    };

} // namespace LaurelEye::Audio
