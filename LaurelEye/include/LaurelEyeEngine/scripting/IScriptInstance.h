/// @file   IScriptInstance.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-7-2025
/// @brief  An actual instance of/reference to a scripting engine script.
/// May be a simple filepath, or if the scripting engine contains a state,
/// may be a state related reference
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

namespace LaurelEye::Physics {
    struct CollisionEventData;
}

namespace LaurelEye::Scripting {

    class IScriptInstance {
    public:
        virtual ~IScriptInstance() = default;

        /// @brief Called once when the script is first created
        virtual void onStart() = 0;

        /// @brief Called once per frame
        virtual void onUpdate(float dt) = 0;

        /// @brief Called when the script is destroyed or the system shuts down
        virtual void onShutdown() = 0;

        /// @brief Called when the script's owner collides with another object
        /// @param data
        virtual void onCollisionEnter(const Physics::CollisionEventData& data) = 0;
        /// @brief Called when the script's owner continues to collide with another object
        /// @param data
        virtual void onCollisionStay(const Physics::CollisionEventData& data) = 0;
        /// @brief Called when the script's owner stops colliding with another object
        /// @param data
        virtual void onCollisionExit(const Physics::CollisionEventData& data) = 0;
    };

} // namespace LaurelEye
