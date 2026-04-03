///// @file   AudioAsset.cpp
///// @author Daoming Wang (KeitherJinn@outlook.com)
///// @par    **DigiPen Email**
/////     daoming.wang@digipen.edu
///// @date    10-08-2025
///// @brief Implementation of Audio asset class for loading and managing audio files.
//
///// @file AudioAsset.cpp
//#include "LaurelEyeEngine/audio/AudioAsset.h"
//#include <stdexcept>
//
//namespace LaurelEye::Audio {
//
//    AudioAsset::AudioAsset(const std::string& path)
//        : path(path), handle(NULL_SOUND), state(AudioState::Unloaded),IAsset(path) {}
//
//    AudioAsset::~AudioAsset() {
//        waitForLoad(); // ensure async task finishes before destruction
//    }
//
//    void AudioAsset::load(IAudioManager& manager, bool is3D, bool loop) {
//        if ( state.load() != AudioState::Unloaded ) return;
//
//        state.store(AudioState::Loading);
//
//        handle = manager.createSound(path, is3D, loop);
//
//        state.store(handle != NULL_SOUND ? AudioState::Ready
//                                         : AudioState::Failed);
//    }
//
//    void AudioAsset::loadAsync(IAudioManager& manager, AudioLoader& loader,
//                               bool is3D, bool loop) {
//        if ( state.load() != AudioState::Unloaded ) return;
//
//        state.store(AudioState::Loading);
//
//        loader.submit([this, &manager, is3D, loop]() {
//            handle = manager.createSoundAsync(path, is3D, loop);
//        });
//    }
//
//    void AudioAsset::waitForLoad() {
//        if ( asyncTask.valid() )
//            asyncTask.get();
//    }
//
//} // namespace LaurelEye::Audio
