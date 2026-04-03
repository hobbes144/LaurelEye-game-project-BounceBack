//#include "LaurelEyeEngine/audio/MusicSystem.h"
//
//namespace LaurelEye::Audio {
//
//    void MusicSystem::play(const std::string& name, float fadeTime) {
//        // If already playing this track, ignore
//        if ( currentTrack == name )
//            return;
//
//        // If fade requested, fade out current track first
//        if ( currentChannel && fadeTime > 0.0f ) {
//            fadeDuration = fadeTime;
//            fadeTimer = 0.0f;
//            fadeStartVolume = currentVolume;
//            fadeTargetVolume = 0.0f;
//
//            // Queue the new track to start after fade
//            musicQueue.push(name);
//            return;
//        }
//
//        // No fade needed — start immediately
//        startTrack(name);
//    }
//
//    void MusicSystem::stop(float fadeTime) {
//        if ( !currentChannel )
//            return;
//
//        if ( fadeTime > 0.0f ) {
//            fadeDuration = fadeTime;
//            fadeTimer = 0.0f;
//            fadeStartVolume = currentVolume;
//            fadeTargetVolume = 0.0f;
//            return;
//        }
//
//        currentChannel->stop();
//        currentChannel = nullptr;
//        currentTrack.clear();
//    }
//
//    void MusicSystem::queue(const std::string& name) {
//        musicQueue.push(name);
//    }
//
//    void MusicSystem::setVolume(float volume) {
//        currentVolume = volume;
//        if ( currentChannel )
//            currentChannel->setVolume(volume);
//    }
//
//    void MusicSystem::startTrack(const std::string& name) {
//        currentTrack = name;
//
//        FMOD::Sound* sound = audio->loadMusic(name);
//        audio->system->playSound(sound, audio->musicGroup, false, &currentChannel);
//
//        currentChannel->setVolume(currentVolume);
//    }
//
//    void MusicSystem::applyFade(float dt) {
//        if ( fadeDuration <= 0.0f )
//            return;
//
//        fadeTimer += dt;
//        float t = fadeTimer / fadeDuration;
//
//        if ( t >= 1.0f ) {
//            // Fade complete
//            currentVolume = fadeTargetVolume;
//            if ( currentChannel )
//                currentChannel->setVolume(currentVolume);
//
//            fadeDuration = 0.0f;
//
//            // If we faded out to zero, stop and play next track
//            if ( currentVolume == 0.0f ) {
//                if ( currentChannel )
//                    currentChannel->stop();
//
//                currentChannel = nullptr;
//                currentTrack.clear();
//
//                if ( !musicQueue.empty() ) {
//                    std::string next = musicQueue.front();
//                    musicQueue.pop();
//                    startTrack(next);
//                }
//            }
//
//            return;
//        }
//
//        // Interpolate volume
//        currentVolume = fadeStartVolume + (fadeTargetVolume - fadeStartVolume) * t;
//
//        if ( currentChannel )
//            currentChannel->setVolume(currentVolume);
//    }
//
//    void MusicSystem::update(float dt) {
//        applyFade(dt);
//
//        // If track ended naturally, play next queued track
//        if ( currentChannel ) {
//            bool playing = false;
//            currentChannel->isPlaying(&playing);
//
//            if ( !playing ) {
//                currentChannel = nullptr;
//                currentTrack.clear();
//
//                if ( !musicQueue.empty() ) {
//                    std::string next = musicQueue.front();
//                    musicQueue.pop();
//                    startTrack(next);
//                }
//            }
//        }
//    }
//
//} // namespace LaurelEye::Audio
