/// @file   TransformComponent.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-24-2025
/// @brief  A hybrid component, containing both self contained logic for
/// transform updates, as well as the pure data needed if logic is handled
/// by the TransformSystem. I.e., this can work both Entity driven or System driven.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/math/Transform.h"

namespace LaurelEye {
    class TransformComponent : public IComponent {
    public:
        TransformComponent()
            : parentTransform(nullptr), dirty(true) {}

        void setIsLocalSpace(bool isLocal) { isLocalSpace = isLocal; }
        bool getIsLocalSpace() const { return isLocalSpace; }

        Transform& getLocalTransform() {
            return localTransform;
        }

        Transform& getWorldTransform() {
            return worldTransform;
        }

        void setLocalTransform(const Transform& t) {
            localTransform = t;
            isLocalSpace = true;
            markDirty(); // Setting a transform requires recomputation
        }
        void setWorldTransform(const Transform& t) {
            worldTransform = t;
            isLocalSpace = false;
            markDirty(); // Setting a transform requires recomputation
        }

        void setParent(TransformComponent* parent) { parentTransform = parent; }
        void addChild(TransformComponent* child) { childTransforms.push_back(child); }

        TransformComponent* getParent() const { return parentTransform; }
        const std::vector<TransformComponent*>& getChildren() const { return childTransforms; }

        void markDirty() {
            if ( dirty ) return;
            dirty = true;
            for ( auto* child : childTransforms ) {
                if ( child ) child->markDirty();
            }
        }

        void markClean() {
            dirty = false;
        }

        bool isDirty() const { return dirty; }

    private:
        bool dirty = true; // Should have its transforms recalculated
        bool isLocalSpace = true;
        Transform localTransform;
        Transform worldTransform;

        // TODO - this kind of breaks pure ECS, since this is technically more than pure data
        // in the future, potentially let system manage hierarchy
        TransformComponent* parentTransform;
        std::vector<TransformComponent*> childTransforms;
    };
} // namespace LaurelEye
