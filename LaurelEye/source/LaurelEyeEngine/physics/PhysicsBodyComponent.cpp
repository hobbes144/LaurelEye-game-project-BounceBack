#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/physics/interfaces/IBody.h"

namespace LaurelEye::Physics {

    PhysicsBodyComponent::PhysicsBodyComponent()
        : data{} {}

    PhysicsBodyComponent::PhysicsBodyComponent(const PhysicsBodyData& initData)
        : data(initData) {}

    std::shared_ptr<IBody> PhysicsBodyComponent::GetBodyRef() {
        return body;
    }

    void PhysicsBodyComponent::SetBodyRef(std::shared_ptr<IBody> b) {
        body = b;
    }

    PhysicsBodyData& PhysicsBodyComponent::GetBodyData() {
        return data;
    }

    void PhysicsBodyComponent::SetBodyData(const PhysicsBodyData& d) {
        data = d;
    }

    void PhysicsBodyComponent::setOwner(Entity* entity) {
        //Call base
        IComponent::setOwner(entity);

        if ( entity ) {
            auto* transform = entity->findComponent<LaurelEye::TransformComponent>();
            if ( transform ) {
                data.transformRef = transform;
            }
        }
    }

    void PhysicsBodyComponent::SetLinearVeloctiy(Vector3 newVelocity) {
        body->SetLinearVelocity(newVelocity);
    }

    Vector3 PhysicsBodyComponent::GetLinearVelocity() {
        return body->GetLinearVelocity();
    }

    void PhysicsBodyComponent::SetAngularVelocity(Vector3 newVelocity) {
        body->SetAngularVelocity(newVelocity);
    }
    Vector3 PhysicsBodyComponent::GetAngularVelocity() {
        return body->GetAngularVelocity();
    }

    void PhysicsBodyComponent::ApplyForce(Vector3 f) {
        body->ApplyForce(f);
    }

    void PhysicsBodyComponent::ApplyImpulse(Vector3 i) {
        body->ApplyImpulse(i);
    }

}
