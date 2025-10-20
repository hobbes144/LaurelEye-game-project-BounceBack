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

#include "LaurelEyeEngine/audio/FModAudioManager.h"

namespace LaurelEye::Audio {
    /*!****************************************************************************
     * \brief To read audio file path configuration.
     *****************************************************************************/
    std::string FModAudioManager::readAudioConfig(const std::string& configFile) {
        std::ifstream file(configFile);
        std::string line;
        std::string key, value;

        if ( !file.is_open() ) {
            std::cerr << "Could not open config file: " << configFile << std::endl;
            return "./"; // fallback path
        }

        while ( std::getline(file, line) ) {
            std::istringstream iss(line);
            if ( std::getline(iss, key, '=') && std::getline(iss, value) ) {
                if ( key == "asset_path" ) {
                    return value;
                }
            }
        }
        return "./"; // default fallback
    }

    /*!****************************************************************************
     * \brief Destructor for AudioManager.
     *
     * ## Usage:
     *
     * This method is automatically triggers when the AudioManager is destroyed.
     *
     * ## Explanation:
     *
     * The destructor make sure the FMOD system is properly shut down by
     * calling shutdown(), releasing all audio resources if the user forgot to do so.
     *****************************************************************************/
    FModAudioManager::~FModAudioManager() {
        shutdown();
    }

    /*!****************************************************************************
     * \brief Initialize the AudioManager.
     *
     * ## Usage:
     *
     * Call this method once at the start of the application to initialize the FMOD
     * audio system before any audio operations are performed.
     *
     * ## Explanation:
     *
     * This method creates and initializes the FMOD system with the specified maximum
     * number of channels.
     *
     * \param maxChannels The maximum number of channels to allocate for audio playback.
     * \return void
     *****************************************************************************/
    void FModAudioManager::initialize(int maxChannels) {
        // Create the main FMOD system object
        FMOD_RESULT result = FMOD::System_Create(&fmodSystem_);
        assert(result == FMOD_OK && fmodSystem_ && "FMOD system creation failed");

        // Initialize the system with a given number of channels
        result = fmodSystem_->init(maxChannels, FMOD_INIT_NORMAL, nullptr);
        assert(result == FMOD_OK && "FMOD system init failed");

        // Example: Set 3D settings if you want realistic attenuation over distance
        fmodSystem_->set3DSettings(
            1.0f, // Doppler scale
            1.0f, // Distance factor (1.0 = 1 meter)
            1.0f  // Rolloff scale
        );

        std::cerr << "[AudioManager] FMOD initialized with " << maxChannels << " channels.\n";
    }

    /*!****************************************************************************
     * \brief Update the FMOD audio system.
     *
     * ## Usage:
     *
     * Call this method once per frame (within the main loop) to allow FMOD to
     * process audio tasks.
     *
     * ## Explanation:
     *
     * This method updates the FMOD system so that the audio can be played
     * every frame.
     *
     * \return void
     *****************************************************************************/
    void FModAudioManager::update() {
        if ( fmodSystem_ ) {
            fmodSystem_->update();
        }
    }

    /*!****************************************************************************
     * \brief Load a sound asset for later playback.
     *
     * ## Usage:
     *
     * Use this method to load a sound file into the AudioManager. This must be done
     * before attempting to play the sound.
     *
     * ## Explanation:
     *
     * This function creates a sound from the specified file path and stores it in a
     * map. The sound can be loaded as either a 3D or 2D sound, and optionally set to loop.
     *
     * \param name A unique identifier for the sound.
     * \param path The file path to the sound asset.
     * \param is3D If true, the sound is loaded as a 3D sound; otherwise as a 2D sound.
     * \param loop If true, the sound will loop over and over again.
     * \return void
     *****************************************************************************/
    void FModAudioManager::loadSound(const std::string& name, const std::string& path, bool is3D, bool loop) {
        assert(fmodSystem_ && "FMOD system not initialized");

        FMOD_MODE mode;
        // Choose either 2D or 3D mode
        if ( is3D ) {
            mode = FMOD_3D;
        }
        else {
            mode = FMOD_2D;
        }

        if ( loop ) {
            mode |= FMOD_LOOP_NORMAL;
        }

        FMOD::Sound* newSound = nullptr;
        FMOD_RESULT result = fmodSystem_->createSound(path.c_str(), mode, nullptr, &newSound);
        assert(result == FMOD_OK && newSound && "Failed to load sound");

        // Store the sound in the map
        sounds_[name] = newSound;
        std::cout << "[AudioManager] Loaded sound \"" << name << "\" from " << path << "\n";
    }

    /*!****************************************************************************
     * \brief Play a previously loaded sound.
     *
     * ## Usage:
     *
     * Call this method to play a sound that has already been loaded using loadSound().
     *
     * ## Explanation:
     *
     * This function retrieves the sound by the map location stored from loadSound()
     * and plays it using the FMOD system. If the sound is 3D, its spatial attributes
     * are set according to the provided position.
     *
     * \param name The unique identifier of the sound to play.
     * \param x The x-coordinate of the sound's position in 3D space.
     * \param y The y-coordinate of the sound's position in 3D space.
     * \param z The z-coordinate of the sound's position in 3D space.
     * \return void
     *****************************************************************************/
    void FModAudioManager::playSound(const std::string& name, const Vector3& position, float volume) {
        // Retrieve the sound pointer from our stored sounds.
        auto it = sounds_.find(name);
        assert(it != sounds_.end() && "Sound not found");
        FMOD::Sound* sound = it->second;
        assert(fmodSystem_ && sound && "Cannot play sound: system or sound invalid");

        // Obtain the master channel group so we can iterate over active channels.
        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        // Iterate over each active channel in the master group.
        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            assert(result == FMOD_OK && "Failed to get channel");
            if ( channel ) {
                FMOD::Sound* currentSound = nullptr;
                // Retrieve the sound currently playing on this channel.
                channel->getCurrentSound(&currentSound);
                // Check if the channel is playing the same sound
                if ( currentSound == sound ) {
                    bool isPlaying = false;
                    channel->isPlaying(&isPlaying);
                    // if the same sound is still playing, do not start a new instance.
                    if ( isPlaying ) {
                        return;
                    }
                }
            }
        }

        // If no active channel is playing the sound, play the sound.
        FMOD::Channel* channel = nullptr;
        result = fmodSystem_->playSound(sound, nullptr, false, &channel);
        assert(result == FMOD_OK && "Failed to play sound");

        // If this is a 3D sound, set its position and attenuation attributes.
        FMOD_MODE currentMode;
        sound->getMode(&currentMode);
        if ( (currentMode & FMOD_3D) == FMOD_3D && channel ) {
            FMOD_VECTOR pos = {position.x, position.y, position.z};
            FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f};
            channel->set3DAttributes(&pos, &vel);
            channel->set3DMinMaxDistance(1.0f, 50.0f);
        }
        if ( channel ) {
            channel->setVolume(volume);
        }
    }

    /*!****************************************************************************
     * \brief Stop playing a specific sound.
     *
     * ## Usage:
     *
     * Call this method with the it's name (as loaded with loadSound())
     * to stop playing the sound.
     *
     * ## Explanation:
     *
     * This function retrieves the stored sound pointer for the given name and fine
     * the channel. If channel name matches the target sound the channel is stopped.
     *
     * \param name The name of the sound to stop.
     * \return void
     *****************************************************************************/
    void FModAudioManager::stopSound(const std::string& name) {
        auto it = sounds_.find(name);
        assert(it != sounds_.end() && "Sound not found");
        FMOD::Sound* targetSound = it->second;

        // Get the master channel group.
        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                FMOD::Sound* playingSound = nullptr;
                channel->getCurrentSound(&playingSound);
                if ( playingSound == targetSound ) {
                    channel->stop();
                }
            }
        }

        std::cout << "[AudioManager] Stopped all instances of sound \"" << name << "\".\n";
    }

    /*!****************************************************************************
     * \brief Set the audio listener's position in 3D space.
     *
     * ## Usage:
     *
     * Call this method every frame (or when the listener's position changes) to update
     * the where you hear the audio. This typically reflects the camera's position.
     *
     * ## Explanation:
     *
     * This function sets the position of the primary audio listener in the FMOD system.
     *
     * \param x The x-coordinate of the listener's position.
     * \param y The y-coordinate of the listener's position.
     * \param z The z-coordinate of the listener's position.
     * \return void
     *****************************************************************************/
    void FModAudioManager::setListenerPosition(const Vector3& position) {
        assert(fmodSystem_ && "FMOD system not initialized in setListenerPosition");

        listenerPosition = position;
        // The orientation vectors can be adjusted based on camera property
        FMOD_VECTOR pos = {position.x, position.y, position.z};
        FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f};
        FMOD_VECTOR forward = {0.0f, 0.0f, 1.0f};
        FMOD_VECTOR up = {0.0f, 1.0f, 0.0f};

        // Set the listener at index 0
        fmodSystem_->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
    }

    /*!****************************************************************************
     * \brief Set the playback speed for all current playing audio.
     *
     * ## Usage:
     *
     * Call this method to adjust the playback speed of all currently playing audio.
     *
     * \param speed A positive float value representing the desired playback speed.
     *              For example, 0.5 for half speed, 2.0 for double speed.
     * \return void
     *****************************************************************************/
    void FModAudioManager::setPlaybackSpeed(float speed) {
        assert(speed > 0.0f && "Playback speed must be a positive value");
        currentPlaybackSpeed_ = speed;

        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        // Iterate over all active channels and set their pitch.
        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                result = channel->setPitch(currentPlaybackSpeed_);
            }
        }
        std::cout << "[AudioManager] Playback speed set to " << speed << "x.\n";
    }

    /*!****************************************************************************
     * \brief Toggle playback speed between normal and a given speed.
     *
     * ## Usage:
     *
     * Call this method to toggle the playback speed between the provided speed and
     * normal speed (1.0). For example, if you want half-speed playback, calling
     * togglePlaybackSpeed(0.5f) will set the speed to 0.5 if it is currently 1.0,
     * or revert to 1.0 if it is already 0.5.
     *
     * \param speed A positive float value representing the alternate playback speed.
     * \return void
     *****************************************************************************/
    void FModAudioManager::togglePlaybackSpeed(float speed) {
        // Use a small epsilon to compare floating-point values.
        const float epsilon = 0.001f;
        if ( std::fabs(currentPlaybackSpeed_ - speed) < epsilon ) {
            setPlaybackSpeed(1.0f);
        }
        else {
            setPlaybackSpeed(speed);
        }
    }

    void FModAudioManager::setVolume(const std::string& name, float volume) {
        auto it = sounds_.find(name);
        assert(it != sounds_.end() && "Sound not found");
        FMOD::Sound* targetSound = it->second;

        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                FMOD::Sound* playingSound = nullptr;
                channel->getCurrentSound(&playingSound);
                if ( playingSound == targetSound ) {
                    result = channel->setVolume(volume);
                    assert(result == FMOD_OK && "Failed to set volume on channel");
                }
            }
        }
        std::cout << "[AudioManager] Volume for sound \"" << name << "\" set to " << volume << ".\n";
    }

    void FModAudioManager::setMasterVolume(float volume) {
        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                result = channel->setVolume(volume);
                assert(result == FMOD_OK && "Failed to set volume on channel");
            }
        }
    }

    // to be implemented later if needed
    void FModAudioManager::playMovingSound(const std::string& name, const Vector3& position, const Vector3& velocity, float volume) {
        return;
    }

    // to be implemented later if needed
    void FModAudioManager::pauseSound(const std::string& name) {
        return;
    }

    // to be implemented later if needed
    void FModAudioManager::resumeSound(const std::string& name) {
        return;
    }

    void FModAudioManager::pauseAllSound() {
        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                result = channel->setPaused(true);
                assert(result == FMOD_OK && "Failed to pause sound");
            }
        }
        std::cout << "[AudioManager] All sound paused.\n";
    }

    void FModAudioManager::resumeAllSound() {
        FMOD::ChannelGroup* masterGroup = nullptr;
        FMOD_RESULT result = fmodSystem_->getMasterChannelGroup(&masterGroup);
        assert(result == FMOD_OK && masterGroup && "Failed to get master channel group");

        int numChannels = 0;
        result = masterGroup->getNumChannels(&numChannels);
        assert(result == FMOD_OK && "Failed to get number of channels");

        for ( int i = 0; i < numChannels; ++i ) {
            FMOD::Channel* channel = nullptr;
            result = masterGroup->getChannel(i, &channel);
            if ( result == FMOD_OK && channel ) {
                result = channel->setPaused(false);
                assert(result == FMOD_OK && "Failed to resume sound");
            }
        }
        std::cout << "[AudioManager] All sound resumed.\n";
    }

    /*!****************************************************************************
     * \brief Shut down the AudioManager and release all resources.
     *
     * ## Usage:
     *
     * Call this method when the application is closing to clean up all audio
     * resources and properly shut down the FMOD system.
     *
     * ## Explanation:
     *
     * This method releases all loaded sound assets and closes the FMOD system to prevent
     * memory leaks. It should be called once before the application exits.
     *
     * \return void
     *****************************************************************************/
    void FModAudioManager::shutdown() {
        // Release all loaded sounds
        for ( auto& pair : sounds_ ) {
            if ( pair.second ) {
                pair.second->release();
                pair.second = nullptr;
            }
        }
        sounds_.clear();

        // Close and release the FMOD system
        if ( fmodSystem_ ) {
            fmodSystem_->close();
            fmodSystem_->release();
            fmodSystem_ = nullptr;
        }

        std::cout << "[AudioManager] FMOD system shut down.\n";
    }
} // namespace LaurelEye::Audio
