#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Transform.h"

namespace LaurelEye::Scripting {
    void Sol2API_Transform::setup(sol::state& lua) {
        lua.new_usertype<TransformComponent>(
        "TransformComponent",
        sol::constructors<TransformComponent()>(),

        // --- Space Type ---
        //"setIsLocalSpace", &TransformComponent::setIsLocalSpace,
        //"getIsLocalSpace", &TransformComponent::getIsLocalSpace,

        // --- Local Transform Access ---
        "getLocalPosition", &TransformComponent::getLocalPosition,
        "setLocalPosition", sol::overload(
            static_cast<void(TransformComponent::*)(const Vector3&)>(&TransformComponent::setLocalPosition),
            [](TransformComponent& self, float x, float y, float z) {
                self.setLocalPosition(Vector3(x, y, z));
            }
        ),
        "getLocalRotation", &TransformComponent::getLocalRotation,
        "setLocalRotation", sol::overload(
            static_cast<void(TransformComponent::*)(const Quaternion&)>(&TransformComponent::setLocalRotation)
        ),
        "getLocalScale", &TransformComponent::getLocalScale,
        "setLocalScale", sol::overload(
            static_cast<void(TransformComponent::*)(const Vector3&)>(&TransformComponent::setLocalScale),
            [](TransformComponent& self, float x, float y, float z) {
                self.setLocalScale(Vector3(x, y, z));
            }
        ),

        // --- World Transform Access ---"
        "getWorldTransform", &TransformComponent::getWorldTransform,
        "setWorldTransform", sol::overload(
            static_cast<void(TransformComponent::*)(const Transform&)>(&TransformComponent::setWorldTransform),
            [](TransformComponent& self, const Transform& tr) {
                self.setWorldTransform(tr);
            }
        ),
        "getWorldPosition", &TransformComponent::getWorldPosition,
        "setWorldPosition", sol::overload(
            static_cast<void(TransformComponent::*)(const Vector3&)>(&TransformComponent::setWorldPosition),
            [](TransformComponent& self, float x, float y, float z) {
                self.setWorldPosition(Vector3(x, y, z));
            }
        ),
        "getWorldRotation", &TransformComponent::getWorldRotation,
        "setWorldRotation", sol::overload(
            static_cast<void(TransformComponent::*)(const Quaternion&)>(&TransformComponent::setWorldRotation)
        ),
        "getWorldScale", &TransformComponent::getWorldScale,
        "setWorldScale", sol::overload(
            static_cast<void(TransformComponent::*)(const Vector3&)>(&TransformComponent::setWorldScale),
            [](TransformComponent& self, float x, float y, float z) {
                self.setWorldScale(Vector3(x, y, z));
            }
        ),

        // --- Hierarchy ---
        "getParent", &TransformComponent::getParent,
        "getChildren", &TransformComponent::getChildren,
        "setParent", &TransformComponent::setParent

        // --- Dirty Flag ---
        //"markDirty", &TransformComponent::markDirty,
        //"markClean", &TransformComponent::markClean,
        //"isDirty", &TransformComponent::isDirty
        );
    }
}
