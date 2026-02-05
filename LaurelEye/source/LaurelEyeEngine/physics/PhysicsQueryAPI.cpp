#include "LaurelEyeEngine/physics/PhysicsSystem.h"

namespace LaurelEye::Physics {
    RaycastHit PhysicsSystem::Raycast(const Vector3& origin,
                                      const Vector3& direction,
                                      float maxDistance,
                                      const RaycastParams& params) const {
        return world->Raycast(origin, direction, maxDistance, params);
    }
} // namespace LaurelEye::Physics
