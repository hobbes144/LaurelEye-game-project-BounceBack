/*!****************************************************************************
 * \file   CollisionManager.cpp
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief
 *
 *****************************************************************************/
#include "LaurelEyeEngine/physics/CollisionManager.h"

namespace LaurelEye::Physics {

    void CollisionManager::beginFrame() {
        currentCollisions.clear();
        events.clear();
    }

    void CollisionManager::registerContact(unsigned int id1,
                                            unsigned int id2,
                                            const Vector3& point,
                                            const Vector3& normal,
                                            const Entity* e1,
                                            const Entity* e2) {
        // Ensure ordering (smallest ID first) for consistency
        if ( id2 < id1 ) std::swap(id1, id2);
        currentCollisions.insert({ id1, id2 });

        // Store optional data (could be expanded per-contact point)
        contactTransformCache[{id1, id2}] = { point, normal };

        // Store
        contactEntityCache[{id1, id2}] = std::make_pair(e1, e2);
    }

    void CollisionManager::endFrame() {
        for ( auto& c : currentCollisions ) {
            const auto& extra = contactTransformCache[c];
            const auto& entities = contactEntityCache[c];

            if ( previousCollisions.find(c) == previousCollisions.end() ) {
                events.emplace_back(
                    CollisionEventData::Type::Enter,
                    c.first, c.second,
                    extra.first, extra.second,
                    const_cast<Entity*>(entities.first),
                    const_cast<Entity*>(entities.second));
            }
            else {
                events.emplace_back(
                    CollisionEventData::Type::Stay,
                    c.first, c.second,
                    extra.first, extra.second,
                    const_cast<Entity*>(entities.first),
                    const_cast<Entity*>(entities.second));
            }
        }

        // Exit collisions
        for ( auto& c : previousCollisions ) {
            if ( currentCollisions.find(c) == currentCollisions.end() ) {
                auto entIt = contactEntityCache.find(c);
                const Entity* e1 = nullptr;
                const Entity* e2 = nullptr;

                if ( entIt != contactEntityCache.end() ) {
                    e1 = entIt->second.first;
                    e2 = entIt->second.second;
                }

                events.emplace_back(
                    CollisionEventData::Type::Exit,
                    c.first, c.second,
                    const_cast<Entity*>(e1),
                    const_cast<Entity*>(e2));

                // Cleanup stale entity pair 
                contactEntityCache.erase(c);
                contactTransformCache.erase(c);
            }
        }

        previousCollisions = currentCollisions;
        contactTransformCache.clear(); // fine to clear frame-local data
        // contactEntityCache stays around for old entries
    }



    const std::vector<CollisionEventData>& CollisionManager::getEvents() const {
        return events;
    }

}
