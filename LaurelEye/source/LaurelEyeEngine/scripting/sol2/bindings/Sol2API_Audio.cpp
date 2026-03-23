#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Audio.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include "LaurelEyeEngine/audio/AudioSystem.h"

namespace LaurelEye::Scripting {
    void Sol2API_Audio::setup(sol::state& lua, EngineContext* ctx) {
        Audio::IAudioManager* audioManager = ctx->getService<Audio::AudioSystem>()->getAudioManager();

        // =========================
        // AudioManager binding
        // =========================
        lua.new_usertype<Audio::IAudioManager>(
            "AudioManager",

            // Master Volume
            "setMasterVolume", &Audio::IAudioManager::setMasterVolume,

            // ADD THIS (you’ll want it)
            "getMasterVolume", [](Audio::IAudioManager& self) {
                // If you don’t already have this in interface, add it (see below)
                return self.getMasterVolume();
            });

        // Global access point in Lua
        lua["Audio"] = audioManager;

        lua.new_usertype<Audio::SpeakerComponent>(
            "SpeakerComponent",
            // Constructors
            sol::constructors<
                Audio::SpeakerComponent(),
                Audio::SpeakerComponent(Audio::IAudioManager*, const std::string&)>(),

            // --- Audio control functions with Lua sugar ---
            "play", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
            if (soundName) self.playSound(*soundName);
            else self.playSound();
            },
            "stop", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
                if (soundName) self.stopSound(*soundName);
                else self.stopSound();
            },
            "pause", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
                if (soundName) self.pauseSound(*soundName);
                else self.pauseSound();
            },
            "resume", [](Audio::SpeakerComponent& self, sol::optional<std::string> soundName) {
                if (soundName) self.resumeSound(*soundName);
                else self.resumeSound();
            },


            // --- Volume ---
            "getVolume", &Audio::SpeakerComponent::getVolume,
            "setVolume", &Audio::SpeakerComponent::setVolume,

            // --- Audio Name ---
            "getAudioName", static_cast<std::string (Audio::SpeakerComponent::*)() const>(&Audio::SpeakerComponent::getAudioName),
            "setAudioName", &Audio::SpeakerComponent::setAudioName,

            // --- Play on load ---
            "getPlayOnLoad", &Audio::SpeakerComponent::getPlayOnLoad,
            "setPlayOnLoad", &Audio::SpeakerComponent::setPlayOnLoad,

            // --- AudioManager ---
            "setAudioManager", &Audio::SpeakerComponent::setAudioManager,

            // --- Asset management ---
            "createAudioAsset", &Audio::SpeakerComponent::createAudioAsset,
            "loadAudioAsset", &Audio::SpeakerComponent::loadAudioAsset,
            "loadAudioAssetImmidiate", &Audio::SpeakerComponent::loadAudioAssetImmidiate
        );



        //lua["SpeakerComponentDefaultManager"] = audioManager;
    }
}
