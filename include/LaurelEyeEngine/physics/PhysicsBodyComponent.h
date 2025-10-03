/// @file   PhysicsBodyComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief ECS component that stores physics body data
/// and a reference to the backend body object.
/// Acts as the link between entities and the physics system.

#pragma once

#include <memory>

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/physics/interfaces/IBody.h"

namespace LaurelEye::Physics {

    class PhysicsBodyComponent : public IComponent {

    public:
        PhysicsBodyComponent();
        explicit PhysicsBodyComponent(const PhysicsBodyData& initData);

        PhysicsBodyData& GetBodyData();
        void SetBodyData(const PhysicsBodyData& d);

        std::shared_ptr<IBody> GetBodyRef();
        void SetBodyRef(std::shared_ptr<IBody> b);

        void setOwner(Entity* entity) override;

    private:
        std::shared_ptr<IBody> body;
        PhysicsBodyData data;
    };

}
