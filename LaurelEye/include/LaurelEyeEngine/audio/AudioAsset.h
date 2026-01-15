/// @file   AudioAsset.h
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief Audio asset class for loading and managing audio files.

#pragma once

#include "LaurelEyeEngine/math/Vector3.h"
#include <atomic>
#include <fmod.hpp>
#include <future>
#include <mutex>
#include <string>

namespace LaurelEye::Audio {
    enum class AudioState {
        Unloaded,
        Loading,
        Ready,
        Failed
    };

    class AudioAsset {
    public:
        AudioAsset();
        AudioAsset(const std::string& _path, float _volume, bool _is3D, bool _loop);
        ~AudioAsset();

        void SetPath(const std::string& newPath) { path = newPath; }
        const std::string& GetPath() const { return path; }
        void SetIs3D(bool _is3D) { is3D = _is3D; }
        bool GetIs3D() const { return is3D; }
        void SetLoop(bool _loop) { loop = _loop; }
        bool GetLoop() const { return loop; }

        void Set3DDistances(float minDist, float maxDist) {
            minDist3D = minDist;
            maxDist3D = maxDist;
        }
        void Set3DMinDistance(float minDist) { minDist3D = minDist; }
        void Set3DMaxDistance(float maxDist) { maxDist3D = maxDist; }

        bool IsPlaying() const { return isPlaying; }

        void SetVelocity(const Vector3& velocity) { movingVelocity = {velocity.x, velocity.y, velocity.z}; }
        void SetPosition(const Vector3& position) { playPosition = {position.x, position.y, position.z}; }
        void SetVolume(float vol);
        float GetVolume() const { return volume; }
        void SetPlayBackSpeed(float speed);

        // check if ready
        bool IsReady() const;

        void Load(FMOD::System* system);
        void LoadAsnyc(FMOD::System* system);
        void Update();

        FMOD::Sound* GetFMODSound() const { return sound; }

        // play this asset (once ready)
        void Play(FMOD::System* system);
        void Stop();
        void Pause();
        void Resume();

    private:
        std::string path;
        bool is3D;
        bool loop;
        FMOD::Sound* sound = nullptr;
        AudioState state;
        float volume;

        FMOD_VECTOR playPosition = {0.0f, 0.0f, 0.0f};
        FMOD_VECTOR movingVelocity = {0.0f, 0.0f, 0.0f};

        float minDist3D = 1.0f;
        float maxDist3D = 50.0f;

        FMOD::Channel* channel = nullptr;

        bool isPlaying;
    };

} // namespace LaurelEye::Audio
