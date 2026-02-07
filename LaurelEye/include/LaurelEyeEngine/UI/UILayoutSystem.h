///
/// @file   UILayoutSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   01-16-2026
/// @brief  
/// 
///

#pragma once

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"

namespace LaurelEye::UI {

    class UILayoutSystem : public ISystem<UITransformComponent> {

    public:
        void initialize() override;
        void update(float dt) override;
        void shutdown() override;

        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(ComponentPtr component) override;

        void Resolve(UITransformComponent& t, Vector2 screenSize);
    private:
        Vector2 storedWindowSize;

    };

}
