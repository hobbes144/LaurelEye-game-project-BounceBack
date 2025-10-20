#include "LaurelEyeEngine/audio/interfaces/IAudioManager.h"
#include "TestDefinitions.h"
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

namespace LaurelEye {
    std::unique_ptr<Audio::FModAudioManager> am;

    void audioPlayTest() {
        /* Audio System Initalization */
        am = std::make_unique<Audio::FModAudioManager>();

        am->update();
        am->loadSound("music", std::string(TEST_MEDIA_DIR) + "/audio/testAudio.mp3", false, true);
        am->playSound("music", Vector3(0.0f, 0.0f, 0.0f), 0.15f);
        am->update();
    }

    void audioStopTest() {
        /* Audio System Initalization */
        am->stopSound("music");
        am->update();
    }

    void audioPauseTest() {
        /* Audio System Initalization */
        am->pauseAllSound();
        am->update();
    }

    void audioResumeTest() {
        /* Audio System Initalization */
        am->resumeAllSound();
        am->update();
    }

    void speakerTest() {
        am->loadSound("slide", std::string(TEST_MEDIA_DIR) + "/audio/slide.mp3", true, true);

        // Create entity
        auto root = std::make_unique<Entity>("RootEntity");
        assert(root != nullptr);
        assert(root->getName() == "RootEntity");

        // Component tests
        TransformComponent* comp = root->addComponent<TransformComponent>();
        assert(comp != nullptr);

        Audio::SpeakerComponent* sp = root->addComponent<Audio::SpeakerComponent>();
        sp->setVolume(0.5f);
        sp->setAudioManager((Audio::IAudioManager*) am.get());
        assert(sp != nullptr);

        // Simple transforms for testing
        // Construct small helper values. Replace Vec/Quat constructors with actual types.
        Vector3 rootPos{5.0f, 0.0f, 0.0f};
        Quaternion rootRot{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 rootScale{1.0f, 1.0f, 1.0f};

        Transform t(rootPos, rootRot, rootScale);
        comp->setOwner(root.get());
        comp->setWorldTransform(t);

        sp->playSound("slide");

        am->update();
    }

    void speakerTest2() {
        am->stopSound("slide");

        // Create entity
        auto root = std::make_unique<Entity>("RootEntity");
        assert(root != nullptr);
        assert(root->getName() == "RootEntity");

        // Component tests
        TransformComponent* comp = root->addComponent<TransformComponent>();
        assert(comp != nullptr);

        Audio::SpeakerComponent* sp = root->addComponent<Audio::SpeakerComponent>();
        sp->setVolume(0.5f);
        sp->setAudioManager((Audio::IAudioManager*) am.get());
        assert(sp != nullptr);

        // Simple transforms for testing
        // Construct small helper values. Replace Vec/Quat constructors with actual types.
        Vector3 rootPos{-5.0f, 0.0f, 0.0f};
        Quaternion rootRot{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 rootScale{1.0f, 1.0f, 1.0f};

        Transform t(rootPos, rootRot, rootScale);
        comp->setOwner(root.get());
        comp->setWorldTransform(t);

        sp->playSound("slide");

        am->update();
    }

    void speakerTest3() {
        am->stopSound("slide");

        am->loadSound("slide2", std::string(TEST_MEDIA_DIR) + "/audio/slide.mp3", true, false);

        // Create entity
        auto root = std::make_unique<Entity>("RootEntity");
        assert(root != nullptr);
        assert(root->getName() == "RootEntity");

        // Component tests
        TransformComponent* comp = root->addComponent<TransformComponent>();
        assert(comp != nullptr);

        Audio::SpeakerComponent* sp = root->addComponent<Audio::SpeakerComponent>();
        assert(sp != nullptr);
        sp->setVolume(1.0f);
        sp->setAudioManager((Audio::IAudioManager*) am.get());

        float x = -10.0f, y = 0.0f, z = 0.0f;

        // Simple transforms for testing
        // Construct small helper values. Replace Vec/Quat constructors with actual types.
        Vector3 rootPos{x, y, z};
        Quaternion rootRot{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 rootScale{1.0f, 1.0f, 1.0f};

        Transform t(rootPos, rootRot, rootScale);
        comp->setOwner(root.get());
        comp->setWorldTransform(t);

        float angle = 0.0f;

        for ( int i = 0; i < 5; i++ ) {
            sp->playSound("slide2");
            // Increase angle over time
            angle += 1 * 0.016f; // assuming ~60 FPS (dt ≈ 16ms)

            if ( angle > 2 * 3.14159 ) angle -= 2 * 3.14159;

            x = 10 * std::cos(angle);
            y = 0;
            z = 10 * std::sin(angle);

            rootPos = {x, y, z};

            t.setPosition(rootPos);
            comp->setWorldTransform(t);
            am->update();

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
} // namespace LaurelEye
