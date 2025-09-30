/// @file   IComponent.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @date   9-21-2025
/// @brief  File containing a Component base class definition. Currently
/// Components can and will execute their own logic (PhysicsComponent executes physics)
/// In the future, this will be mainly data driven.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

namespace LaurelEye {
    class Entity;
    class IComponent {
    public:
        virtual ~IComponent() = default;

        Entity* getOwner() const { return owner; }
        virtual void setOwner(Entity* entity) { owner = entity; }

        bool isActive() const { return active; }
        void setIsActive(bool isActive) { active = isActive; } 

    protected:
        bool active{true};
        Entity* owner{nullptr};
    };
}
