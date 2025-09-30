/// @file   TransformSystem.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-24-2025
/// @brief  A system to handle updating of Transforms for all objects in the given scene.
/// NOTE - currently, hierarchy must be honored when registering components, as the
/// system will not natively support it
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/ecs/ISystem.h"

namespace LaurelEye {
    class TransformComponent;
    class TransformSystem : public ISystem<TransformComponent> {
    public:
        void initialize() override;
        void update(float deltaTime) override;
        void shutdown() override;

    private:
        void updateSubtree(TransformComponent* rootLevelComponent);
        // Use this when utilizing pure ECS
        void updateTransforms(TransformComponent* comp);
    };
} // namespace LaurelEye
