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

#include "LaurelEyeEngine/math/Vector3.h"
#include <string>

namespace LaurelEye::Audio {
    class IAudioManager {
    public:
        virtual ~IAudioManager() = default;

        virtual void update() = 0;
        virtual void shutdown() = 0;

        virtual void createSound(const std::string& name, const std::string& path, float volume = 1.0f, bool is3D = false, bool loop = false) = 0;
        virtual void loadSound(const std::string& name) = 0;
        virtual void loadSoundImmidiate(const std::string& name) = 0;
        virtual void playSound(const std::string& name) = 0;
        virtual void stopSound(const std::string& name) = 0;

        virtual void setListenerPosition(const Vector3& position) = 0;
        virtual Vector3 getListenerPosition() const = 0;
        virtual void setListenerVelocity(const Vector3& velocity) = 0;
        virtual Vector3 getListenerVelocity() const = 0;

        virtual void setPlaybackSpeed(float speed) = 0;
        virtual void togglePlaybackSpeed(float speed = 1) = 0;
        virtual void setVolume(const std::string& name, float volume) = 0;
        virtual float getVolume(const std::string& name) const = 0;
        virtual void setSoundPosition(const std::string& name, const Vector3& position) = 0;
        virtual void setSoundVelocity(const std::string& name, const Vector3& velocity) = 0;
        virtual float getMasterVolume() = 0;
        virtual void setMasterVolume(float volume) = 0;
        virtual void pauseSound(const std::string& name) = 0;
        virtual void resumeSound(const std::string& name) = 0;
        virtual void pauseAllSound() = 0;
        virtual void resumeAllSound() = 0;

        virtual std::string readAudioConfig(const std::string& configFilePath) = 0;

    protected:
        float masterVolume = 100.0f;
    };
} // namespace LaurelEye::Audio

