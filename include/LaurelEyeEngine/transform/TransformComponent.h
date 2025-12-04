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

#include <cassert>

namespace LaurelEye {
    class TransformComponent : public IComponent {
    public:
        TransformComponent()
            : parentTransform(nullptr), worldDirty(true) {}

        Transform& getLocalTransform() {
            updateLocalTransforms();

            return localTransform;
        }
        void setLocalTransform(const Transform& t) {
            updateLocalTransforms();
            localTransform = t;
            markWorldDirty(); // Setting a transform requires recomputation
        }

        Transform& getWorldTransform() {
            updateWorldTransforms();

            return worldTransform;
        }
        void setWorldTransform(const Transform& t) {
            updateWorldTransforms();
            worldTransform = t;
            markLocalDirty();
            dirtyDescendents();
        }

        // --- Local Position / Rotation / Scale ---
        Vector3 getLocalPosition() {
            updateLocalTransforms();
            return localTransform.getPosition();
        }
        void setLocalPosition(const Vector3& pos) {
            updateLocalTransforms();
            localTransform.setPosition(pos);
            markWorldDirty();
        }

        Quaternion getLocalRotation() {
            updateLocalTransforms();
            return localTransform.getRotation();
        }
        void setLocalRotation(const Quaternion& rot) {
            updateLocalTransforms();
            localTransform.setRotation(rot);
            markWorldDirty();
        }

        void setLocalRotation(const Vector3& rot) {
            updateLocalTransforms();
            localTransform.setRotation(rot);
            markWorldDirty();
        }

        Vector3 getLocalScale() {
            updateLocalTransforms();
            return localTransform.getScaling();
        }
        void setLocalScale(const Vector3& scale) {
            updateLocalTransforms();
            localTransform.setScaling(scale);
            markWorldDirty();
        }

        // --- World Position / Rotation / Scale ---
        // Must call updateTransforms() before calling this
        void setAbsoluteWorldPosRot(const Vector3& pos, const Quaternion& rot) {
            // updateLocalTransforms();
            assert(!isLocalDirty() && "Physics update called on component that isn't updated");
            worldTransform.setPosition(pos);
            worldTransform.setRotation(rot);
            markLocalDirty();
            dirtyDescendents();
            markWorldClean();
        }

        Vector3 getWorldPosition() {
            updateWorldTransforms();
            return worldTransform.getPosition();
        }
        void setWorldPosition(const Vector3& pos) {
            updateWorldTransforms();

            worldTransform.setPosition(pos);
            markLocalDirty();
            dirtyDescendents();
        }

        Quaternion getWorldRotation() {
            updateWorldTransforms();
            return worldTransform.getRotation();
        }
        void setWorldRotation(const Quaternion& rot) {
            updateWorldTransforms();
            worldTransform.setRotation(rot);
            markLocalDirty();
            dirtyDescendents();
        }

        void setWorldRotation(const Vector3& rot) {
            updateLocalTransforms();

            if ( !parentTransform ) {
                localTransform.setRotation(rot);
                worldTransform.setRotation(rot);
                return;
            }

            Transform parentWorldTransform = parentTransform->getWorldTransform();
            localTransform.setRotation(parentWorldTransform.getRotation().inverse() * rot);
            worldTransform.setRotation(rot);
        }

        Vector3 getWorldScale() {
            updateWorldTransforms();
            return worldTransform.getScaling();
        }
        void setWorldScale(const Vector3& scale) {
            updateWorldTransforms();
            worldTransform.setScaling(scale);
            markLocalDirty();
            dirtyDescendents();
        }

        void setParent(TransformComponent* newParent) {
            updateTransforms();

            TransformComponent* currParent = parentTransform;
            if ( currParent ) {
                // 1. erase the found node from parent's children vector and update sibling numbers
                unsigned int removedIndex = siblingNumber;
                currParent->getChildren().erase(currParent->getChildren().begin() + removedIndex);
                // still have a reference shared in foundNode variable
                currParent->updateSiblingNumbers(removedIndex);
            }

            // 2. attach this to dstNode
            newParent->addChild(this);

            markLocalDirty();
        }
        void addChild(TransformComponent* child) {
            child->setSiblingNumber(childTransforms.size());
            childTransforms.push_back(child);
        }

        TransformComponent* getParent() const { return parentTransform; }
        // const std::vector<TransformComponent*>& getChildren() const { return childTransforms; }
        std::vector<TransformComponent*>& getChildren() { return childTransforms; }

        void updateSiblingNumbers(unsigned int removedIndex) {

            for ( unsigned int i = removedIndex; i < childTransforms.size(); i++ ) {
                childTransforms[i]->setSiblingNumber(i);
            }
        }

        void setSiblingNumber(unsigned int n) {
            siblingNumber = n;
        }

        unsigned int getSiblingNumber() {
            return siblingNumber;
        }

        void markLocalDirty() {
            localDirty = true;
        }

        void markWorldDirty() {
            worldDirty = true;
        }

        void markLocalClean() {
            localDirty = false;
        }

        void markWorldClean() {
            worldDirty = false;
        }

        bool isLocalDirty() const { return localDirty; }

        bool isWorldDirty() const { return worldDirty; }

        void dirtyDescendents(bool prop = true) {
            for ( TransformComponent* tChild : childTransforms ) {
                // tChild->markLocalDirty();
                tChild->markWorldDirty();
                tChild->dirtyDescendents();
            }
        }

        void updateTransforms() {
            assert(!(isLocalDirty() && isWorldDirty()) && "[TransformComponent.h] Both Local & World are Dirty (No Ground Truth)");

            updateWorldTransforms();
            updateLocalTransforms();
        }

        void updateLocalTransforms() {
            if ( isLocalDirty() ) {
                if ( parentTransform ) {

                    Transform parentWorldTransform = parentTransform->getWorldTransform();
                    Transform newLocalTransform(
                        parentWorldTransform.getInverseLocalMatrix() * worldTransform.getPosition(),
                        worldTransform.getRotation() * parentWorldTransform.getRotation().inverse(),
                        worldTransform.getScaling() * parentWorldTransform.getScaling().reciprocal());
                    setLocalTransform(newLocalTransform);
                }
                else {
                    localTransform = worldTransform;
                }
                markLocalClean();
            }
        }
        void updateWorldTransforms() {
            if ( isWorldDirty() ) {
                if ( parentTransform ) {
                    Transform parentWorldTransform = parentTransform->getWorldTransform();
                    worldTransform = parentWorldTransform * localTransform;
                }
                else {
                    worldTransform = localTransform;
                }
                markWorldClean();
            }
        }

    private:
        bool localDirty = false;
        bool worldDirty = true; // Should have its transforms recalculated

        Transform localTransform;
        Transform worldTransform;

        unsigned int siblingNumber = 0;

        // TODO - this kind of breaks pure ECS, since this is technically more than pure data
        // in the future, potentially let system manage hierarchy
        TransformComponent* parentTransform;
        std::vector<TransformComponent*> childTransforms;
    };
} // namespace LaurelEye
