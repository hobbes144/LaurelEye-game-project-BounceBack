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
#include "LaurelEyeEngine/io/FileSystem.h"

//#define NDEBUG

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
        fmodSystem_->setStreamBufferSize(1024 * 1024, FMOD_TIMEUNIT_RAWBYTES);

        // Example: Set 3D settings if you want realistic attenuation over distance
        fmodSystem_->set3DSettings(
            1.0f, // Doppler scale
            1.0f, // Distance factor (1.0 = 1 meter)
            1.0f  // Rolloff scale
        );
#if !defined(NDEBUG)
        std::cerr << "[AudioManager] FMOD initialized with " << maxChannels << " channels.\n";
#endif
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
            for ( const auto& pair : sounds_ ) {
                sounds_[pair.first]->Update();
            }
            if ( awatingSounds.empty() == false ) {
                for ( int i = 0; i < awatingSounds.size(); ++i ) {
                    AudioAsset* sound = awatingSounds[i];
                    if ( sound->IsReady() ) {
                        sound->Play(fmodSystem_);
                        awatingSounds.erase(awatingSounds.begin() + i);
                        --i;
                    }
                    else {
#if !defined(NDEBUG)
                        std::cout << "[AudioManager] Sound in the awating queue is not ready to play yet.\n";
#endif
                    }
                }
            }
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
        auto fullPath = IO::resolve(path);

        AudioAsset* newAsset = new AudioAsset(fullPath.string(), is3D, loop);
        newAsset->LoadAsnyc(fmodSystem_);

        // Store the sound in the map
        sounds_[name] = newAsset;
#if !defined(NDEBUG)
        std::cout << "[AudioManager] Loaded sound \"" << name << "\" from " << path << "\n";
#endif
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
        AudioAsset* sound = it->second;
        assert(fmodSystem_ && sound && "Cannot play sound: system or sound invalid");

        if ( sound->IsPlaying() ) {
            return;
        }
        else {
            if ( sound->IsReady() ) {
                sound->SetVolume(volume);
                sound->SetPosition(position);
                sound->Play(fmodSystem_);
            }
            else {
                sound->SetPosition(position);
                sound->SetVolume(volume);
                awatingSounds.push_back(sound);
#if !defined(NDEBUG)
                std::cout << "[AudioManager] Sound \"" << name << "\" is not ready to play yet.\n";
#endif
            }
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
        AudioAsset* targetSound = it->second;
        targetSound->Stop();

#if !defined(NDEBUG)
        std::cout << "[AudioManager] Stopped all instances of sound \"" << name << "\".\n";
#endif
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

        for ( const auto& pair : sounds_ ) {
            sounds_[pair.first]->SetVolume(currentPlaybackSpeed_);
        }

#if !defined(NDEBUG)
        std::cout << "[AudioManager] Playback speed set to " << speed << "x.\n";
#endif
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
        AudioAsset* targetSound = it->second;
        targetSound->SetVolume(volume);

#if !defined(NDEBUG)
        std::cout << "[AudioManager] Volume for sound \"" << name << "\" set to " << volume << ".\n";
#endif
    }

    void FModAudioManager::setMasterVolume(float volume) {
        for ( const auto& pair : sounds_ ) {
            sounds_[pair.first]->SetVolume(volume);
        }
    }

    // to be implemented later if needed
    void FModAudioManager::playMovingSound(const std::string& name, const Vector3& position, const Vector3& velocity, float volume) {
        return;
    }

    void FModAudioManager::pauseSound(const std::string& name) {
        auto it = sounds_.find(name);
        assert(it != sounds_.end() && "Sound not found");
        AudioAsset* targetSound = it->second;
        targetSound->Pause();
#if !defined(NDEBUG)
        std::cout << "[AudioManager] Paused sound \"" << name << "\" .\n";
#endif
        return;
    }

    void FModAudioManager::resumeSound(const std::string& name) {
        auto it = sounds_.find(name);
        assert(it != sounds_.end() && "Sound not found");
        AudioAsset* targetSound = it->second;
        targetSound->Resume();
#if !defined(NDEBUG)
        std::cout << "[AudioManager] Resumed sound \"" << name << "\" .\n";
#endif
        return;
    }

    void FModAudioManager::pauseAllSound() {
        for ( const auto& pair : sounds_ ) {
            sounds_[pair.first]->Pause();
        }
#if !defined(NDEBUG)
        std::cout << "[AudioManager] All sound paused.\n";
#endif
    }

    void FModAudioManager::resumeAllSound() {
        for ( const auto& pair : sounds_ ) {
            sounds_[pair.first]->Resume();
        }
#if !defined(NDEBUG)
        std::cout << "[AudioManager] All sound resumed.\n";
#endif
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
        sounds_.clear();

        // Close and release the FMOD system
        if ( fmodSystem_ ) {
            fmodSystem_->close();
            fmodSystem_->release();
            fmodSystem_ = nullptr;
        }
#if !defined(NDEBUG)
        std::cout << "[AudioManager] FMOD system shut down.\n";
#endif
    }
} // namespace LaurelEye::Audio
