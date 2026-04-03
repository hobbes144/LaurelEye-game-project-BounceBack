#pragma once
/// @file ListenerComponent.h
#pragma once
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Audio {

    /// @brief Attach to the player or camera entity.
    ///        AudioSystem reads this each frame to update FMOD's listener.
    ///        Only ONE active listener is supported at a time.
    class ListenerComponent {
    public:
        void update();
        owner->findComponent<TransformComponent>();
        bool isActive() const { return active; }
        void setActive(bool a) { active = a; }

    private:

        Vector3 position{};
        Vector3 velocity{};
        Vector3 forward{0.f, 0.f, -1.f}; // sensible defaults
        Vector3 up{0.f, 1.f, 0.f};
        bool active = true;
    };

} // namespace LaurelEye::Audio
