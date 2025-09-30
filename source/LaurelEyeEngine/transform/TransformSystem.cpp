#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye {
    void TransformSystem::initialize() {
        // setup transform world
    }

    void TransformSystem::update(float deltaTime) {
        for ( auto* comp : components ) {
            if ( comp->isActive() && comp->getParent() == nullptr ) {
                updateSubtree(comp);
            }
        }
    }

    void TransformSystem::shutdown() {
        // cleanup
    }

    void TransformSystem::updateSubtree(TransformComponent* comp) {
        if ( comp->isDirty() ) {
            updateTransforms(comp);
            comp->markClean();
        }

        for ( auto* child : comp->getChildren() ) {
            if ( child->isActive() ) {
                updateSubtree(child);
            }
        }
    }

    void TransformSystem::updateTransforms(TransformComponent* comp) {
        TransformComponent* parent = comp->getParent();
        if ( parent ) {
            if ( comp->getIsLocalSpace() ) {
                comp->setWorldTransform(parent->getWorldTransform() * comp->getLocalTransform());
            }
            else {
                Transform parentWorldTransform = parent->getWorldTransform();
                Transform worldTransform = comp->getWorldTransform();
                Transform newLocalTransform(
                    parentWorldTransform.getInverseLocalMatrix() * worldTransform.getPosition(),
                    worldTransform.getRotation() * parentWorldTransform.getRotation().inverse(),
                    worldTransform.getScaling() * parentWorldTransform.getScaling().reciprocal());
                comp->setLocalTransform(newLocalTransform);
                comp->setIsLocalSpace(true);
            }
        }
        else { // Allow for root entities
            if ( comp->getIsLocalSpace() )
                comp->setWorldTransform(comp->getLocalTransform());
            else {
                comp->setLocalTransform(comp->getWorldTransform());
                comp->setIsLocalSpace(true);
            }
        }
    }
} // namespace LaurelEye
