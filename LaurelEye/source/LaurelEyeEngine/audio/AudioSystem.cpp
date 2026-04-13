/// @file   AudioSystem.cpp
/// @author Daoming Wang (KeitherJinn@outlook.com),Martin Chow(chowmartin387@gmail.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   20-3-2025
/// @brief  Implementation of AudioSystem that manages audio components and interfaces with audio backends.

#include "LaurelEyeEngine/audio/AudioSystem.h"


namespace LaurelEye::Audio {

    static SoundCategory convertCategory(SoundType type) {
        switch ( type ) {
        case SoundType::BGM:
            return SoundCategory::Music;
        case SoundType::UI:
            return SoundCategory::UI;
        case SoundType::SFX2D:
        case SoundType::SFX3D:
        default:
            return SoundCategory::SFX;
        }
    }

    AudioSystem::AudioSystem(IAudioManager* manager, AudioSystemType type)
        : audioManager(manager),
          systemType(type) {
    }

    void AudioSystem::initialize() {
        // too complex and too much work, and it does not improve performace and design
        // replace with Audio Manager instead
        //auto sfxImporter = std::make_shared<LaurelEye::IO::AudioAssetImporter>(
        //    *audioManager, audioLoader, LaurelEye::IO::LoadHint::Sync);

        //auto bgmImporter = std::make_shared<LaurelEye::IO::AudioAssetImporter>(
        //    *audioManager, audioLoader, LaurelEye::IO::LoadHint::Async);
        //auto* assetManager = context->getService<IO::AssetManager>();
        //// Register importers for .wav and .mp3 files
        //assetManager->registerImporter(".wav", sfxImporter);
        //// For simplicity, we treat .mp3 as BGM and load it asynchronously
        //assetManager->registerImporter(".mp3", bgmImporter);
    }

    void AudioSystem::update(float dt) {
        updateComponents(dt); // iterate ISystem<SpeakerComponent>'s list
        audioManager->update(); // FMOD internal tick — always last
    }

    void AudioSystem::updateComponents(float dt) {
        for ( auto* comp : components ) {
            if ( !comp || !comp->isActive() )
                continue;

            AudioComponent* audio = static_cast<AudioComponent*>(comp);
            // 1. Sync 3D position from entity transform(existing)
            updateSpatialAudio(*audio);

            // 2. Update each sound entry (play, stop, fade, loop)
            updateSoundEntries(*audio, dt);
        }
    }

    void AudioSystem::updateSpatialAudio(AudioComponent& audio) {
        const Vector3& pos = audio.getPosition();
        const Vector3& vel = audio.getVelocity();

        for ( auto& [id, entry] : audio.getSounds() ) {
            if ( entry.type == SoundType::SFX3D && entry.channelHandle != NULL_CHANNEL ) {
                audioManager->setChannel3DAttributes(entry.channelHandle, pos, vel);
            }
        }
    }

    void AudioSystem::updateSoundEntries(AudioComponent& audio, float dt) {
        for ( auto& [id, entry] : audio.getSounds() ) {

            // A. If active but no channel yet → play it
            if ( entry.active && entry.channelHandle == NULL_CHANNEL ) {

                bool is3D = (entry.type == SoundType::SFX3D);

                SoundHandle sound = audioManager->createSound(
                    entry.path,
                    is3D,
                    entry.looping);

                SoundCategory category = convertCategory(entry.type);

                entry.channelHandle = audioManager->playSound(
                    sound,
                    entry.volume,
                    category);

                if ( is3D && entry.channelHandle != NULL_CHANNEL ) {
                    audioManager->setChannel3DAttributes(
                        entry.channelHandle,
                        audio.getPosition(),
                        audio.getVelocity());
                }
            }

            // B. If fading in
            if ( entry.fadingIn && entry.channelHandle != NULL_CHANNEL ) {
                entry.currentVolume += entry.fadeSpeed * dt;

                if ( entry.currentVolume >= entry.volume ) {
                    entry.currentVolume = entry.volume;
                    entry.fadingIn = false;
                }

                audioManager->setChannelVolume(entry.channelHandle, entry.currentVolume);
            }

            // C. If fading out
            if ( entry.fadingOut && entry.channelHandle != NULL_CHANNEL ) {
                entry.currentVolume -= entry.fadeSpeed * dt;

                if ( entry.currentVolume <= 0.0f ) {
                    audioManager->stopChannel(entry.channelHandle);
                    entry.channelHandle = NULL_CHANNEL;
                    entry.fadingOut = false;
                }
                else {
                    audioManager->setChannelVolume(entry.channelHandle, entry.currentVolume);
                }
            }

            // D. If channel finished → clear handle
            // fire and forget 3D sound will not have handle, so we dont need to check them
            if ( entry.channelHandle != NULL_CHANNEL &&
                 !audioManager->isChannelPlaying(entry.channelHandle) ) {
                entry.channelHandle = NULL_CHANNEL;

                //this is only for looping 3D sound like the machine, if we really need
                if ( !entry.looping )
                    entry.active = false; // fire-and-forget
            }

        }
    }

    // --- Global Volume Controls ---

    void AudioSystem::setMasterVolume(float volume) {
        audioManager->setGroupVolume(audioManager->getMusicGroup(), volume);
        audioManager->setGroupVolume(audioManager->getSFXGroup(), volume);
        audioManager->setGroupVolume(audioManager->getUIGroup(), volume);
        masterVolume = volume;
    }

    void AudioSystem::setMusicVolume(float volume) {
        audioManager->setGroupVolume(audioManager->getMusicGroup(), volume);
    }

    void AudioSystem::setSFXVolume(float volume) {
        audioManager->setGroupVolume(audioManager->getSFXGroup(), volume);
    }

    void AudioSystem::setUIVolume(float volume) {
        audioManager->setGroupVolume(audioManager->getUIGroup(), volume);
    }

    float AudioSystem::getMasterVolume() const {
        return masterVolume;
    }

    float AudioSystem::getMusicVolume() const {
        return audioManager->getGroupVolume(audioManager->getMusicGroup());
    }

    float AudioSystem::getSFXVolume() const {
        return audioManager->getGroupVolume(audioManager->getSFXGroup());
    }

    float AudioSystem::getUIVolume() const {
        return audioManager->getGroupVolume(audioManager->getUIGroup());
    }


    // --- Global Pause Controls ---

    void AudioSystem::pauseMaster(bool paused) {
        audioManager->pauseGroup(audioManager->getMusicGroup(), paused);
        audioManager->pauseGroup(audioManager->getSFXGroup(), paused);
        audioManager->pauseGroup(audioManager->getUIGroup(), paused);
    }

    void AudioSystem::pauseMusic(bool paused) {
        audioManager->pauseGroup(audioManager->getMusicGroup(), paused);
    }

    void AudioSystem::pauseSFX(bool paused) {
        audioManager->pauseGroup(audioManager->getSFXGroup(), paused);
    }

    void AudioSystem::pauseUI(bool paused) {
        audioManager->pauseGroup(audioManager->getUIGroup(), paused);
    }

    // --- Optional: Global Mute Controls ---

    void AudioSystem::muteMaster(bool muted) {
        float v = muted ? 0.0f : 1.0f;
        setMasterVolume(v);
    }

    void AudioSystem::muteMusic(bool muted) {
        float v = muted ? 0.0f : 1.0f;
        setMusicVolume(v);
    }

    void AudioSystem::muteSFX(bool muted) {
        float v = muted ? 0.0f : 1.0f;
        setSFXVolume(v);
    }

    void AudioSystem::muteUI(bool muted) {
        float v = muted ? 0.0f : 1.0f;
        setUIVolume(v);
    }

    // --- Listener Controls ---

    void AudioSystem::setListenerPosition(const Vector3& position) {
        listenerPosition = position;
        audioManager->setListenerAttributes(
            listenerPosition,
            listenerVelocity,
            listenerForward);
    }

    void AudioSystem::setListenerVelocity(const Vector3& velocity) {
        listenerVelocity = velocity;
        audioManager->setListenerAttributes(
            listenerPosition,
            listenerVelocity,
            listenerForward);
    }

    void AudioSystem::setListenerForward(const Vector3& forward) {
        listenerForward = forward;
        audioManager->setListenerAttributes(
            listenerPosition,
            listenerVelocity,
            listenerForward);
    }


    // --- Really Optional: Global Fade Controls ---

    void AudioSystem::fadeOutAll(float speed) {
        // You can implement this later by storing fade state in AudioSystem
        // and gradually reducing group volumes in update().
        // I dont want to right now lazy
    }

    void AudioSystem::fadeInAll(float speed) {
        // Same idea as fadeOutAll
    }

    AudioSystem::~AudioSystem() {
        shutdown();
    }

    void AudioSystem::shutdown() {
        audioManager->shutdown();
        components.clear();
    }
}
