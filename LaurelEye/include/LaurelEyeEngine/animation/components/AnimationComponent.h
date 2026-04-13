#pragma once

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/animation/resources/Animation.h"


namespace LaurelEye::Animations {

    struct AnimationComponent : public IComponent {
        AnimationComponent(Animation::Type _type = Animation::Type::Basic) : type(_type)
        {
        }

        AnimationHandle currentAnimation = Animation::InvalidAnimation;
        Animation::Type type;
        double elapsedTime = 0.0;
        bool isPlaying = true;

    };

} // namespace LaurelEye
