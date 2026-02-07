///
/// @file   UIInteractableSystem.h
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
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"

namespace LaurelEye::UI {

    class UIInteractionSystem : public ISystem<UIInteractionComponent> {

    };

}
