#pragma once
#include "FModAudioManager.h"
#include <queue>
#include <string>

//namespace LaurelEye::Audio {
//
//    class MusicSystem {
//    public:
//        MusicSystem(FModAudioManager* backend)
//            : audio(backend) {}
//
//        void update(float dt);
//
//        // Public API
//        void play(const std::string& name, float fadeTime = 0.0f);
//        void stop(float fadeTime = 0.0f);
//        void queue(const std::string& name);
//        void setVolume(float volume);
//        float getVolume() const { return currentVolume; }
//
//    private:
//        // Internal helpers
//        void startTrack(const std::string& name);
//        void applyFade(float dt);
//
//        // Backend
//        FModAudioManager* audio;
//
//        // FMOD channel for current music
//        FMOD::Channel* currentChannel = nullptr;
//
//        // Queued next track
//        std::queue<std::string> musicQueue;
//
//        // Fade state
//        float fadeTimer = 0.0f;
//        float fadeDuration = 0.0f;
//        float fadeStartVolume = 1.0f;
//        float fadeTargetVolume = 1.0f;
//
//        // Current volume
//        float currentVolume = 1.0f;
//
//        // Currently playing track name
//        std::string currentTrack;
//    };
//
//} // namespace LaurelEye::Audio
