/// @file AudioComponent.cpp
#include "LaurelEyeEngine/audio/AudioComponent.h"

namespace LaurelEye::Audio {

    void AudioComponent::addSound(const std::string& id,
                                  const std::string& path,
                                  SoundType type,
                                  float volume,
                                  float fadeSpeed,
                                  bool looping) 
    {
        SoundEntry entry;
        entry.path = path;
        entry.type = type;
        entry.volume = volume;
        entry.fadeSpeed = fadeSpeed;
        entry.looping = looping;
        entry.currentVolume = volume;
        entry.active = false;
        entry.fadingIn = false;
        entry.fadingOut = false;
        entry.channelHandle = NULL_CHANNEL;

        sounds[id] = entry;
    }


    void AudioComponent::removeSound(const std::string& id) {
        sounds.erase(id);
    }

    void AudioComponent::play(const std::string& id) {
        auto it = sounds.find(id);
        if ( it == sounds.end() )
            return;

        SoundEntry& entry = it->second;
        if ( entry.active == true)
        {
            return;
        }
        entry.active = true;
        entry.fadingOut = false;

        if ( entry.fadeSpeed > 0.0f) {
            entry.fadingIn = true;
            entry.currentVolume = 0.0f;
        }
        else {
            entry.fadingIn = false;
            entry.currentVolume = entry.volume;
        }
    }

    void AudioComponent::stop(const std::string& id) {
        auto it = sounds.find(id);
        if ( it == sounds.end() )
            return;

        SoundEntry& entry = it->second;

        // Mark inactive so AudioSystem won't restart it
        entry.active = false;

        // Trigger fade-out if enabled
        if ( entry.fadeSpeed > 0.0f ) {
            entry.fadingOut = true;
            entry.fadingIn = false;
        }
        else {
            // No fade → AudioSystem will stop it next update
            entry.fadingOut = false;
            entry.fadingIn = false;
            entry.currentVolume = 0.0f;
        }
    }

    void AudioComponent::stopAll() {
        for ( auto& [id, entry] : sounds ) {
            entry.active = false;

            if ( entry.channelHandle != NULL_CHANNEL ) {
                audioManager->stopChannel(entry.channelHandle);
            }
            entry.channelHandle = NULL_CHANNEL;

        }
    }


} // namespace LaurelEye::Audio
