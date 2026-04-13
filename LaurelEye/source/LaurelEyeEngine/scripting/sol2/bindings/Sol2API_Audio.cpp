#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Audio.h"
#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include "LaurelEyeEngine/audio/AudioSystem.h"
#include "LaurelEyeEngine/audio/AudioComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_Audio::setup(sol::state& lua, EngineContext* ctx) {
        //FModAudioManager* audioManager = ctx->getService<FModAudioManager>()
        Audio::AudioSystem* audioSystem = ctx->getService<Audio::AudioSystem>();

        lua["Audio"] = lua.create_table();

        lua["Audio"]["setMasterVolume"] = [audioSystem](float v) {
            audioSystem->setMasterVolume(v);
        };

        lua["Audio"]["setMusicVolume"] = [audioSystem](float v) {
            audioSystem->setMusicVolume(v);
        };

        lua["Audio"]["setSFXVolume"] = [audioSystem](float v) {
            audioSystem->setSFXVolume(v);
        };

        lua["Audio"]["setUIVolume"] = [audioSystem](float v) {
            audioSystem->setUIVolume(v);
        };

        lua["Audio"]["pauseMaster"] = [audioSystem](bool p) {
            audioSystem->pauseMaster(p);
        };

        lua["Audio"]["pauseMusic"] = [audioSystem](bool p) {
            audioSystem->pauseMusic(p);
        };

        lua["Audio"]["pauseSFX"] = [audioSystem](bool p) {
            audioSystem->pauseSFX(p);
        };

        lua["Audio"]["pauseUI"] = [audioSystem](bool p) {
            audioSystem->pauseUI(p);
        };

        lua["Audio"]["setListenerPosition"] = [audioSystem](float x, float y, float z) {
            audioSystem->setListenerPosition({x, y, z});
        };

        lua["Audio"]["setListenerVelocity"] = [audioSystem](float x, float y, float z) {
            audioSystem->setListenerVelocity({x, y, z});
        };

        lua["Audio"]["setListenerForward"] = [audioSystem](float x, float y, float z) {
            audioSystem->setListenerForward({x, y, z});
        };


        lua.new_enum("SoundType",
                     "SFX2D", LaurelEye::Audio::SoundType::SFX2D,
                     "SFX3D", LaurelEye::Audio::SoundType::SFX3D,
                     "BGM", LaurelEye::Audio::SoundType::BGM,
                     "UI", LaurelEye::Audio::SoundType::UI);

        lua.new_usertype<LaurelEye::Audio::AudioComponent>("AudioComponent",
                                                           sol::constructors<LaurelEye::Audio::AudioComponent(LaurelEye::Audio::IAudioManager*)>(),

                                         "addSound", &LaurelEye::Audio::AudioComponent::addSound,
                                                           "removeSound", &LaurelEye::Audio::AudioComponent::removeSound,
                                                           "play", &LaurelEye::Audio::AudioComponent::play,
                                                           "stop", &LaurelEye::Audio::AudioComponent::stop,
                                                           "stopAll", &LaurelEye::Audio::AudioComponent::stopAll,

                                         "setPosition", &LaurelEye::Audio::AudioComponent::setPosition,
                                                           "setVelocity", &LaurelEye::Audio::AudioComponent::setVelocity);


        //lua.new_usertype<Audio::SpeakerComponent>(
        //    "SpeakerComponent",
        //    // Constructors
        //    sol::constructors<
        //        Audio::SpeakerComponent(),
        //        Audio::SpeakerComponent(Audio::IAudioManager*, const std::string&)>(),

        //    // --- Audio control functions with Lua sugar ---
        //    "play", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
        //    if (soundName) self.playSound(*soundName);
        //    else self.playSound();
        //    },
        //    "stop", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
        //        if (soundName) self.stopSound(*soundName);
        //        else self.stopSound();
        //    },
        //    "pause", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
        //        if (soundName) self.pauseSound(*soundName);
        //        else self.pauseSound();
        //    },
        //    "resume", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
        //        if (soundName) self.resumeSound(*soundName);
        //        else self.resumeSound();
        //    },


        //    // --- Volume ---
        //    "getVolume", &Audio::SpeakerComponent::getVolume,
        //    "setVolume", &Audio::SpeakerComponent::setVolume,

        //    // --- Audio Name ---
        //    "getAudioName", static_cast<std::string (Audio::SpeakerComponent::*)() const>(&Audio::SpeakerComponent::getAudioName),
        //    "setAudioName", &Audio::SpeakerComponent::setAudioName,

        //    // --- Play on load ---
        //    "getPlayOnLoad", &Audio::SpeakerComponent::getPlayOnLoad,
        //    "setPlayOnLoad", &Audio::SpeakerComponent::setPlayOnLoad,

        //    // --- AudioManager ---
        //    "setAudioManager", &Audio::SpeakerComponent::setAudioManager,

        //    // --- Asset management ---
        //    "createAudioAsset", &Audio::SpeakerComponent::createAudioAsset,
        //    "loadAudioAsset", &Audio::SpeakerComponent::loadAudioAsset,
        //    "loadAudioAssetImmidiate", &Audio::SpeakerComponent::loadAudioAssetImmidiate);

        //lua["SpeakerComponentDefaultManager"] = audioManager;
    }
}
