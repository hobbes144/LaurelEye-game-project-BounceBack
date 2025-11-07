/// @file   AudioAsset.cpp
/// @author Daoming Wang (KeitherJinn@outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-08-2025
/// @brief

#include "LaurelEyeEngine/audio/AudioAsset.h"

//#define NDEBUG

namespace LaurelEye::Audio {
    AudioAsset::AudioAsset()
        : path(""), is3D(false), loop(false), sound(nullptr), state(AudioState::Unloaded), isPlaying(false) {
    }

    AudioAsset::AudioAsset(const std::string& _path, bool _is3D, bool _loop)
        : path(_path), is3D(_is3D), loop(false), sound(nullptr), state(AudioState::Unloaded), isPlaying(false) {
    }

    AudioAsset::~AudioAsset() {
        if ( sound ) {
            sound->release();
            sound = nullptr;
        }
    }

    void AudioAsset::Initialize(const std::string& _path, bool _is3D, bool _loop) {
        path = _path;
        is3D = _is3D;
        loop = _loop;
        state = AudioState::Unloaded;
        isPlaying = false;
    }

    void AudioAsset::Stop() {
        if ( sound && isPlaying ) {
            channel->stop();
            isPlaying = false;
        }
    }

    void AudioAsset::Pause() {
        if ( sound && isPlaying ) {
            channel->setPaused(true);
            isPlaying = false;
        }
    }

    void AudioAsset::Resume() {
        if ( sound && !isPlaying ) {
            channel->setPaused(false);
            isPlaying = true;
        }
    }

    void AudioAsset::Load(FMOD::System* system) {
        if ( state != AudioState::Unloaded && state != AudioState::Failed ) {
#if !defined(NDEBUG)
            std::cout << "[AudioManager] Sound already loaded or in invalid state: " << path << "\n";
#endif
            return;
        }
        state = AudioState::Loading;
        FMOD_MODE mode;
        // Choose either 2D or 3D mode
        if ( is3D ) {
            mode = FMOD_3D;
        }
        else {
            mode = FMOD_2D;
        }
        if ( loop ) {
            mode |= FMOD_LOOP_NORMAL | FMOD_ACCURATETIME;
        }

        FMOD_RESULT result = system->createSound(path.c_str(), mode, nullptr, &sound);
        if ( result == FMOD_OK && sound ) {
            state = AudioState::Ready;
        }
        else {
#if !defined(NDEBUG)
            std::cout << "[AudioManager] Failed to load sound from " << path << "\n";
#endif
            state = AudioState::Failed;
        }
    }

    void AudioAsset::LoadAsnyc(FMOD::System* system) {
        if ( state != AudioState::Unloaded && state != AudioState::Failed ) {
#if !defined(NDEBUG)
            std::cout << "[AudioManager] Sound already loaded or in invalid state: " << path << "\n";
#endif
            return;
        }
        state = AudioState::Loading;

        FMOD_MODE mode;
        // Choose either 2D or 3D mode
        if ( is3D ) {
            mode = FMOD_CREATESTREAM | FMOD_NONBLOCKING | FMOD_3D;
        }
        else {
            mode = FMOD_CREATESTREAM | FMOD_NONBLOCKING | FMOD_2D;
        }
        if ( loop ) {
            mode |= FMOD_LOOP_NORMAL | FMOD_ACCURATETIME;
        }

        FMOD_CREATESOUNDEXINFO ex = {};
        ex.cbsize = sizeof(ex);
        // 128 KB decode chunks (try 64K–256K)
        ex.decodebuffersize = 256 * 1024;

        FMOD_RESULT result = system->createSound(path.c_str(), mode, &ex, &sound);
        if ( result == FMOD_OK && sound ) {
            state = AudioState::Loading;
        }
        else {
#if !defined(NDEBUG)
            std::cout << "[AudioManager] Failed to load sound asynchronously from " << path << "\n";
#endif
            state = AudioState::Failed;
        }
    }

    void AudioAsset::Update() {
        if ( state != AudioState::Loading ) {
            return;
        }
        // Check if the sound is ready
        FMOD_OPENSTATE openState;
        unsigned int percent = 0;
        bool starving = false;
        bool diskBusy = false;
        FMOD_RESULT result = sound->getOpenState(&openState, &percent, &starving, &diskBusy);
#if !defined(NDEBUG)
        std::cout << "[AudioManager] Sound loading " << percent << "%, " << openState << " " << starving << " " << diskBusy << "\n ";
#endif
        if ( result == FMOD_OK ) {
            if ( openState == FMOD_OPENSTATE_READY ) {
                state = AudioState::Ready;
            }
            else if ( openState == FMOD_OPENSTATE_ERROR ) {
                state = AudioState::Failed;
            }
        }
    }

    void AudioAsset::SetVolume(float vol) {
        volume = vol;
        if ( channel ) {
            channel->setVolume(volume);
        }
    }

    void AudioAsset::SetPlayBackSpeed(float speed) {
        if ( channel ) {
            channel->setPitch(speed);
        }
    }

    bool AudioAsset::IsReady() const {
        return state == AudioState::Ready;
    }

    void AudioAsset::Play(FMOD::System* system) {
        if ( IsReady() ) {
            FMOD_RESULT result = system->playSound(sound, nullptr, false, &channel);
            assert(result == FMOD_OK && "Failed to play sound");
            isPlaying = true;

            // If this is a 3D sound, set its position and attenuation attributes.
            FMOD_MODE currentMode;
            sound->getMode(&currentMode);
            if ( (currentMode & FMOD_3D) == FMOD_3D && channel ) {
                channel->set3DAttributes(&playPosition, &movingVelocity);
                channel->set3DMinMaxDistance(minDist3D, maxDist3D);
            }
            if ( channel ) {
                channel->setVolume(volume);
                if ( loop ) {
                    channel->setMode(FMOD_LOOP_NORMAL);
                    channel->setLoopCount(-1);
                }
            }
        }
    }
} // namespace LaurelEye::Audio
