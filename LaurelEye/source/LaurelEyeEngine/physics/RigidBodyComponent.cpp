#include "LaurelEyeEngine/physics/RigidBodyComponent.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/math/Transform.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"

namespace LaurelEye::Physics {

    RigidBodyComponent::RigidBodyComponent(){}

    RigidBodyComponent::RigidBodyComponent(const PhysicsBodyData& initData)
    {
        PhysicsBodyBaseComponent::data = initData;
    }

    //--Override Functionality
    std::shared_ptr<ICollider> RigidBodyComponent::GetCollider() const {
        return rigidBody;
    }

    void RigidBodyComponent::CreateInWorld(PhysicsSystem& system) {
        //system.registerComponent(this);

        rigidBody = std::dynamic_pointer_cast<IRigidBody>(system.CreateCollider(data));

        if ( !rigidBody ) {
            std::cerr << "[RigidBodyComponent::CreateInWorld] : INVALID RIGIDBODY CAST\n";
            return;
        }

        // Bind the Body to its Component
        rigidBody->BindTransform(data.transformRef);
        rigidBody->SetUserData(this);
    }
    void RigidBodyComponent::RemoveFromWorld(PhysicsSystem& system) {

        if ( !rigidBody ) return;

        //rigidBody->BindPhysicsBodyComponent(nullptr);
        system.GetWorld()->RemoveObject(rigidBody);
        rigidBody->BindTransform(nullptr);
        rigidBody.reset();

    }
    
    void RigidBodyComponent::SyncToPhysics() {
        if ( rigidBody ) rigidBody->pushTransformToPhysics();
    }
    void RigidBodyComponent::SyncFromPhysics() {
        if ( rigidBody ) rigidBody->updateTransformFromPhysics();
    }

    //--Rigidbody Functionality
    /*
    std::shared_ptr<IBody> RigidBodyComponent::GetBodyRef() {
        return body;
    }

    void RigidBodyComponent::SetBodyRef(std::shared_ptr<IBody> b) {
        body = b;
    }
    */

    void RigidBodyComponent::setOwner(Entity* entity) {
        //Call base 
        PhysicsBodyBaseComponent::setOwner(entity);

        if ( entity ) {
            auto* transform = entity->findComponent<LaurelEye::TransformComponent>();
            if ( transform ) {
                data.transformRef = transform;
            }
        }
    }

    void RigidBodyComponent::SetLinearVeloctiy(Vector3 newVelocity) {
        rigidBody->SetLinearVelocity(newVelocity);
    }

    Vector3 RigidBodyComponent::GetLinearVelocity() {
        return rigidBody->GetLinearVelocity();
    }

    void RigidBodyComponent::SetAngularVelocity(Vector3 newVelocity) {
        rigidBody->SetAngularVelocity(newVelocity);
    }
    Vector3 RigidBodyComponent::GetAngularVelocity() {
        return rigidBody->GetAngularVelocity();
    }

    void RigidBodyComponent::ApplyForce(Vector3 f) {
        rigidBody->ApplyForce(f);
    }

    void RigidBodyComponent::ApplyImpulse(Vector3 i) {
        rigidBody->ApplyImpulse(i);
    }

}
