/// @file   CollisionManager.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/25/2025
/// @brief Determines which enetities are colliding and collects
/// the proper Collision Event Data
#pragma once

#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/math/Vector3.h"

#include <set>
#include <map>

namespace LaurelEye::Physics {

    /// @brief The Data of a Collision Event
    struct CollisionEventData {
        /// @brief Type of Collision Event (Enter, Stay, Exit)
        enum class Type {
            ERROR,
            Enter,
            Stay,
            Exit
        };
        enum class Interaction {
            ERROR,
            Collision,
            Trigger
        };
        Type type = Type::ERROR;
        Interaction interaction = Interaction::ERROR;
        unsigned int entityAID = 0;
        unsigned int entityBID = 0;
        Entity* entityARef = nullptr;
        Entity* entityBRef = nullptr;
        Vector3 contactPoint = Vector3(0);  // optional
        Vector3 contactNormal = Vector3(0); // optional

        CollisionEventData() = default;

        /// @brief Complete Parmeterized constructor
        /// @param t Type of Collision Event
        /// @param aID Unique ID of the Physics Body A
        /// @param bID Unique ID of the Physics Body B
        /// @param point The Vector3 Position of contant
        /// @param normal The Vector3 Normal Direction of the Collision event
        /// @param aRef A Reference to EntityA
        /// @param bRef A Reference to EntityB
        CollisionEventData(Type t,
                           Interaction i,
                           unsigned int aID,
                           unsigned int bID,
                           const Vector3& point,
                           const Vector3& normal,
                           Entity* aRef = nullptr,
                           Entity* bRef = nullptr)
            : type(t),
              interaction(i),
              entityAID(aID),
              entityBID(bID),
              contactPoint(point),
              contactNormal(normal),
              entityARef(aRef),
              entityBRef(bRef) {}

        /// @brief Simplified Constructor for no contact Point (Exit Events)
        /// @param t Type of Collision Event
        /// @param aID Unique ID of the Physics Body A
        /// @param bID Unique ID of the Physics Body B
        /// @param aRef A Reference to EntityA
        /// @param bRef A Reference to EntityB
        CollisionEventData(Type t,
                           Interaction i,
                           unsigned int aID,
                           unsigned int bID,
                           Entity* aRef = nullptr,
                           Entity* bRef = nullptr)
            : type(t),
              interaction(i),
              entityAID(aID),
              entityBID(bID),
              entityARef(aRef),
              entityBRef(bRef) {}
    };

    /// @brief Inline osrtream<< Operator for ease of printing CollisionEventData and documentation for debugging
    inline std::ostream& operator<<(std::ostream& os, const CollisionEventData& c) {
        os << "PhysicsContact { "

           << "Interaction: ";

        switch ( c.interaction ) {
        case CollisionEventData::Interaction::Collision:
            os << "Collision, ";
            break;
        case CollisionEventData::Interaction::Trigger:
            os << "Trigger, ";
            break;
        case CollisionEventData::Interaction::ERROR:
            os << "ERROR, ";
            break;
        default:
            break;
        }

        os << "Type: ";

        switch ( c.type ) {
        case CollisionEventData::Type::Enter:
            os << "Enter, ";
            break;
        case CollisionEventData::Type::Exit:
            os << "Exit, ";
            break;
        case CollisionEventData::Type::Stay:
            os << "Stay, ";
            break;
        default:
            break;
        }

        os << "BodyA: " << c.entityAID << ", "
           << "BodyB: " << c.entityBID << ", "
           << "Point: (" << c.contactPoint.x << ", "
           << c.contactPoint.y << ", "
           << c.contactPoint.z << "), "
           << "Normal: (" << c.contactNormal.x << ", "
           << c.contactNormal.y << ", "
           << c.contactNormal.z << ") }";
        return os;
    }

    /// @brief CollisionManager manages the current and previous collisions from frame
    ///        to frame and stores the relevant event Data
    class CollisionManager {

    public:
        /// @brief Called at the start of a physics fram
        void beginFrame();
        /// @brief Registers a contact to current collisions
        ///        Utilized by IWorld to gather collisions
        /// @param id1 Unique ID of physics object 1 
        /// @param id2 Unique ID of physics object 2
        /// @param point Vector3 point of Contant
        /// @param normal Vector3 normal vector of contact
        /// @param e1 Reference to Entity 1
        /// @param e2 Reference to Entity 2
        void registerContact(
            unsigned int id1,
            unsigned int id2,
            CollisionEventData::Interaction interaction,
            const Vector3& point = Vector3(),
            const Vector3& normal = Vector3(),
            const Entity* e1 = nullptr,
            const Entity* e2 = nullptr);
        /// @brief Called at the end of the Physics frame
        void endFrame();

        //void GatherContatcts();

        /// @brief Returns CollisionEventData for all active events
        /// @return std::vector<CollisionEventData> of all active events
        const std::vector<CollisionEventData>& getEvents() const;

    private:
        std::set<std::pair<unsigned int, unsigned int>> currentCollisions;
        std::set<std::pair<unsigned int, unsigned int>> previousCollisions;

        std::map<std::pair<unsigned int, unsigned int>,
                 std::pair<Vector3, Vector3>>
            contactTransformCache;

        std::map<std::pair<unsigned int, unsigned int>,
                 std::pair<const Entity*, const Entity*>>
            contactEntityCache;

        std::map<std::pair<unsigned int, unsigned int>,
                 CollisionEventData::Interaction>
            interactionCache;

        std::vector<CollisionEventData> events;
    };

}

#endif // !COLLISION_MANAGER_H
