#include "LaurelEyeEngine/physics/GhostBodyComponent.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Physics {

    GhostBodyComponent::GhostBodyComponent(const PhysicsBodyData& d) {
        data = d;
    }

    std::shared_ptr<ICollider> GhostBodyComponent::GetCollider() const {
        return ghost;
    }

    void GhostBodyComponent::CreateInWorld(PhysicsSystem& system) {
        if ( ghost ) return;

        ghost = std::dynamic_pointer_cast<IGhostBody>(system.CreateCollider(data));

        if ( !ghost ) {
            LE_DEBUG_ERROR("Physics", "Invalid GhostBody cast.");
            return;
        }

        ghost->BindTransform(data.transformRef);
        ghost->SetUserData(this);
        //ghost = system.GetWorld()->CreateGhostBody(data);
    }

    void GhostBodyComponent::RemoveFromWorld(PhysicsSystem& system) {
        if ( !ghost ) return;

        system.GetWorld()->RemoveObject(ghost);
        ghost.reset();
    }

    void GhostBodyComponent::SyncToPhysics() {
        if (ghost) ghost->pushTransformToPhysics();
    }

    void GhostBodyComponent::SyncFromPhysics() {

        if (ghost) ghost->updateTransformFromPhysics();
    }

    void GhostBodyComponent::setOwner(Entity* entity) {
        // Call base
        PhysicsBodyBaseComponent::setOwner(entity);

        if ( entity ) {
            auto* transform = entity->findComponent<LaurelEye::TransformComponent>();
            if ( transform ) {
                data.transformRef = transform;
            }
        }
    }

    void GhostBodyComponent::SetLinearVeloctiy(Vector3 newVelocity) {
        ghost->SetLinearVelocity(newVelocity);
    }

    Vector3 GhostBodyComponent::GetLinearVelocity() {
        return ghost->GetLinearVelocity();
    }

    void GhostBodyComponent::SetAngularVelocity(Vector3 newVelocity) {
        ghost->SetAngularVelocity(newVelocity);
    }
    Vector3 GhostBodyComponent::GetAngularVelocity() {
        return ghost->GetAngularVelocity();
    }

    void GhostBodyComponent::ApplyForce(Vector3 f) {
        //ghost->ApplyForce(f);
        //std::cerr << "[GhostBodyComponent::ApplyForce] Cannot apply Force to GhostBody" << std::endl;
        LE_DEBUG_WARN("Physics", "Cannot apply Force to GhostBody");
    }

    void GhostBodyComponent::ApplyImpulse(Vector3 i) {
        //ghost->ApplyImpulse(i);
        //std::cerr << "[GhostBodyComponent::ApplyImplus] Cannot apply Impulse to GhostBody" << std::endl;
        LE_DEBUG_WARN("Physics", "Cannot apply Impulse to GhostBody");
    }

    Vector3 GhostBodyComponent::getGravityScale() {
        return ghost->GetGravityScale();
    }
    void GhostBodyComponent::setGravityScale(Vector3 nv) {
        ghost->SetGravityScale(nv);
    }

}
