#pragma once
#include "LaurelEyeEngine/audio/AudioComponent.h"
/// @file BackgroundMusicComponent.h
class BackgroundMusicComponent : public AudioComponent {
public:
    void play(IAudioManager& manager) override {
        if ( soundHandle == NULL_SOUND ) return;
        channelHandle = manager.playSound(soundHandle, 0.0f); // start silent, fade in
        fadingIn = true;
    }

    void stop(IAudioManager& manager) override {
        fadingOut = true; // don't stop immediately, fade out first
    }

    void update(IAudioManager& manager, float dt) override {
        if ( fadingIn ) {
            currentVolume = std::min(currentVolume + fadeSpeed * dt, volume);
            manager.setChannelVolume(channelHandle, currentVolume);
            if ( currentVolume >= volume ) fadingIn = false;
        }

        if ( fadingOut ) {
            currentVolume = std::max(currentVolume - fadeSpeed * dt, 0.0f);
            manager.setChannelVolume(channelHandle, currentVolume);
            if ( currentVolume <= 0.0f ) {
                manager.stopChannel(channelHandle);
                channelHandle = NULL_CHANNEL;
                fadingOut = false;
            }
        }
    }

    void setFadeSpeed(float speed) { fadeSpeed = speed; }

private:
    float currentVolume = 0.0f;
    float fadeSpeed = 1.0f; // units per second
    bool fadingIn = false;
    bool fadingOut = false;
};
