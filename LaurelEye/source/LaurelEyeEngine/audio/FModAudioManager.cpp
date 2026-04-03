/*!****************************************************************************
 * \file   Audio.cpp
 * \author Daoming Wang (KeitherJinn@outlook.com)
 * \par    **DigiPen Email**
 *    daoming.wang@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-29-2025
 * \brief  FMOD Audio manager (and other supporting classes) implementation.
 *
 *****************************************************************************/

/// @file   FModAudioManager.cpp
#include "LaurelEyeEngine/audio/FModAudioManager.h"
#include "LaurelEyeEngine/io/FileSystem.h"
#include <cassert>

namespace LaurelEye::Audio {

    FModAudioManager::FModAudioManager(int maxChannels)
        : maxChannels(maxChannels) {}

    FModAudioManager::~FModAudioManager() {
        shutdown();
    }

    void FModAudioManager::initialize() {
        FMOD_RESULT result = FMOD::System_Create(&system);
        // change to LE assert later
        assert(result == FMOD_OK && "FMOD: System_Create failed");

        // glm style right-handed 3D coordinates, which is what we're using in our math library(I think)
        result = system->init(maxChannels, FMOD_INIT_3D_RIGHTHANDED, nullptr);
        assert(result == FMOD_OK && "FMOD: system->init failed");

        // default is good enough for our needs, but we could tweak these if we want to optimize for latency or memory usage
        //system_->setStreamBufferSize(1024 * 1024, FMOD_TIMEUNIT_RAWBYTES);

        createDefaultGroups();
    }

    void FModAudioManager::createDefaultGroups() {
        system->getMasterChannelGroup(&masterGroup);

        system->createChannelGroup("Music", &musicGroup);
        system->createChannelGroup("SFX", &sfxGroup);
        system->createChannelGroup("UI", &uiGroup);

        // Register handles
        masterGroupHandle = registerGroup(masterGroup);
        musicGroupHandle = registerGroup(musicGroup);
        sfxGroupHandle = registerGroup(sfxGroup);
        uiGroupHandle = registerGroup(uiGroup);

        // Attach groups to master
        masterGroup->addGroup(musicGroup);
        masterGroup->addGroup(sfxGroup);
        masterGroup->addGroup(uiGroup);
    }

    ChannelGroupHandle FModAudioManager::registerGroup(FMOD::ChannelGroup* group) {
        assert(group != nullptr && "FMOD: Tried to register a null channel group");
        ChannelGroupHandle handle = nextGroupHandle++;
        groups[handle] = group;
        return handle;
    }

    FMOD::ChannelGroup* FModAudioManager::getGroup(ChannelGroupHandle handle) {
        auto it = groups.find(handle);
        return (it != groups.end()) ? it->second : nullptr;
    }



    void FModAudioManager::update() {
        if ( system ) {
            pruneFinishedChannels();
            system->update();
        }
    }

    void FModAudioManager::shutdown() {
        if ( !system ) return;

        // Stop all audio first
        if ( masterGroup )
            masterGroup->stop();

        // Release channel groups we created
        if ( musicGroup ) musicGroup->release();
        if ( sfxGroup ) sfxGroup->release();
        if ( uiGroup ) uiGroup->release();

        // Release all loaded sounds
        for ( auto& [handle, sound] : sounds )
            sound->release();
        sounds.clear();

        // FMOD owns channels; we just drop our references
        channels.clear();
        groups.clear();

        system->close();
        system->release();
        system = nullptr;
    }

    // --- Sound creation ---
    SoundHandle FModAudioManager::createSound(const std::string& path,
                                              bool is3D,
                                              bool loop) {
        FMOD_MODE mode = FMOD_DEFAULT;
        mode |= is3D ? FMOD_3D : FMOD_2D;
        mode |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

        FMOD::Sound* sound = nullptr;
        auto string = "games/sample/assets/" + path;
        auto fullPath = IO::resolve(string).make_preferred().string();
        std::cout << "Loading sound: " << fullPath << std::endl;

        FMOD_RESULT result = system->createSound(fullPath.c_str(), mode, nullptr, &sound);
        if ( result != FMOD_OK )
        {
            // Log the error (in a real implementation, you'd want a proper logging system)
            fprintf(stderr, "FMOD: Failed to create sound '%s' (error %d )\n",
                    path.c_str(), result);
            return NULL_SOUND;
        }
            

        SoundHandle handle = nextSoundHandle++;
        sounds[handle] = sound;
        return handle;
    }

    SoundHandle FModAudioManager::createSoundAsync(const std::string& path,
                                                   bool is3D,
                                                   bool loop) {
        FMOD_MODE mode = FMOD_DEFAULT | FMOD_NONBLOCKING;
        mode |= is3D ? FMOD_3D : FMOD_2D;
        mode |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

        // recomment for large file
        mode |= FMOD_CREATESTREAM;

        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result = system->createSound(path.c_str(), mode, nullptr, &sound);
        if ( result != FMOD_OK )
            return NULL_SOUND;

        SoundHandle handle = nextSoundHandle++;
        sounds[handle] = sound;
        return handle;
    }

    SoundLoadState FModAudioManager::getSoundLoadState(SoundHandle handle) {
        auto it = sounds.find(handle);
        if ( it == sounds.end() ) return SoundLoadState::Failed;

        FMOD_OPENSTATE openState;
        unsigned int percentBuffered;
        bool starving, diskBusy;

        //instead of doing it with hard code change we can get the state though FMOD API
        // less work to do and more accurate
        FMOD_RESULT result = it->second->getOpenState(&openState,
                                                      &percentBuffered,
                                                      &starving,
                                                      &diskBusy);
        if ( result != FMOD_OK ) return SoundLoadState::Failed;

        switch ( openState ) {
        case FMOD_OPENSTATE_READY:
            return SoundLoadState::Ready;
        case FMOD_OPENSTATE_ERROR:
            return SoundLoadState::Failed;
        default:
            return SoundLoadState::Loading;
        }
    }

    void FModAudioManager::destroySound(SoundHandle handle) {
        auto it = sounds.find(handle);
        if ( it == sounds.end() ) return;

        it->second->release();
        sounds.erase(it);
    }

    // --- Playback ---
    ChannelHandle FModAudioManager::playSound(SoundHandle soundHandle,
                                              float volume,
                                              SoundCategory category) {
        auto it = sounds.find(soundHandle);
        if ( it == sounds.end() )
            return NULL_CHANNEL;

        FMOD::Sound* sound = it->second;

        // Check async load state
        FMOD_OPENSTATE state;
        sound->getOpenState(&state, nullptr, nullptr, nullptr);

        if ( state == FMOD_OPENSTATE_ERROR )
            return NULL_CHANNEL;

        FMOD::Channel* channel = nullptr;

        // Play the sound
        FMOD_RESULT result = system->playSound(sound, nullptr, false, &channel);
        if ( result != FMOD_OK || !channel )
            return NULL_CHANNEL;

        // Route to correct category group
        FMOD::ChannelGroup* targetGroup = nullptr;
        switch ( category ) {
        case SoundCategory::Music:
            targetGroup = musicGroup;
            break;
        case SoundCategory::UI:
            targetGroup = uiGroup;
            break;
        default:
            targetGroup = sfxGroup;
            break;
        }

        if ( targetGroup )
            channel->setChannelGroup(targetGroup);

        // Set initial volume
        channel->setVolume(volume);

        channel->setPaused(false);

        // Register channel handle
        ChannelHandle handle = nextChannelHandle++;
        channels[handle] = channel;

        return handle;
    }

    void FModAudioManager::playOneShot3D(SoundHandle sound,
                                         const Vector3& pos,
                                         const Vector3& vel,
                                         float volume,
                                         SoundCategory category) {
        // Validate sound
        auto it = sounds.find(sound);
        if ( it == sounds.end() )
            return;

        FMOD::Sound* fmodSound = it->second;

        // Play the sound
        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = system->playSound(fmodSound, nullptr, false, &channel);
        if ( result != FMOD_OK || !channel )
            return;

        // Route to correct group
        FMOD::ChannelGroup* targetGroup = nullptr;
        switch ( category ) {
        case SoundCategory::Music:
            targetGroup = musicGroup;
            break;
        case SoundCategory::UI:
            targetGroup = uiGroup;
            break;
        default:
            targetGroup = sfxGroup;
            break;
        }

        if ( targetGroup )
            channel->setChannelGroup(targetGroup);

        // Set volume
        channel->setVolume(volume);

        // Apply 3D attributes
        FMOD_VECTOR fpos{pos.x, pos.y, pos.z};
        FMOD_VECTOR fvel{vel.x, vel.y, vel.z};
        channel->set3DAttributes(&fpos, &fvel);

        // Fire-and-forget: do NOT store the channel handle
        // FMOD will clean it up automatically when finished
    }


    void FModAudioManager::stopChannel(ChannelHandle handle) {
        auto it = channels.find(handle);
        if ( it == channels.end() ) return;

        it->second->stop();
        channels.erase(it);
    }

    void FModAudioManager::pauseChannel(ChannelHandle handle, bool paused) {
        auto it = channels.find(handle);
        if ( it == channels.end() ) return;

        it->second->setPaused(paused);
    }

    void FModAudioManager::setChannelVolume(ChannelHandle handle, float volume) {
        auto it = channels.find(handle);
        if ( it == channels.end() ) return;

        it->second->setVolume(volume);
    }

    bool FModAudioManager::isChannelPlaying(ChannelHandle handle) {
        auto it = channels.find(handle);
        if ( it == channels.end() ) return false;

        bool playing = false;
        it->second->isPlaying(&playing);
        return playing;
    }

    void FModAudioManager::setGroupVolume(ChannelGroupHandle group, float volume) {
        if ( auto* g = getGroup(group) )
            g->setVolume(volume);
    }

    void FModAudioManager::pauseGroup(ChannelGroupHandle group, bool paused) {
        if ( auto* g = getGroup(group) )
            g->setPaused(paused);
    }


    // --- 3D attributes ---

    void FModAudioManager::setChannel3DAttributes(ChannelHandle handle,
                                                  const Vector3& position,
                                                  const Vector3& velocity) {
        auto it = channels.find(handle);
        if ( it == channels.end() ) return;

        FMOD_VECTOR fpos{position.x, position.y, position.z};
        FMOD_VECTOR fvel{velocity.x, velocity.y, velocity.z};

        it->second->set3DAttributes(&fpos, &fvel);
    }

    void FModAudioManager::setListenerAttributes(const Vector3& position,
                                                 const Vector3& velocity,
                                                 const Vector3& forward) {
        FMOD_VECTOR fpos{position.x, position.y, position.z};
        FMOD_VECTOR fvel{velocity.x, velocity.y, velocity.z};
        FMOD_VECTOR fwd{forward.x, forward.y, forward.z};
        FMOD_VECTOR fup{0.0f, 1.0f, 0.0f};

        system->set3DListenerAttributes(0, &fpos, &fvel, &fwd, &fup);
    }

    // --- Private helpers ---

    void FModAudioManager::pruneFinishedChannels() {
        for ( auto it = channels.begin(); it != channels.end(); ) {

            if ( !it->second ) {
                it = channels.erase(it);
                continue;
            }

            bool playing = false;
            FMOD_RESULT result = it->second->isPlaying(&playing);

            if ( result != FMOD_OK || !playing )
                it = channels.erase(it);
            else
                ++it;
        }
    }

} // namespace LaurelEye::Audio
