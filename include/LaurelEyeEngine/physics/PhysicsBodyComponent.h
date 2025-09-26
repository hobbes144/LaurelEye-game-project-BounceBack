#pragma once

#include <memory>

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/physics/interfaces/IBody.h"

namespace LaurelEye::Physics {

    class PhysicsBodyComponent : LaurelEye::IComponent {

        public:
        PhysicsBodyComponent();

        void initialize() override;
        void update(float dt) override;
        void shutdown() override;

        private:
        std::shared_ptr<IBody> body;

    };

}
