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
                                            CollisionEventData::Interaction interaction,
                                            const Vector3& point,
                                            const Vector3& normal,
                                            const Entity* e1,
                                            const Entity* e2) {
        // Ensure ordering (smallest ID first) for consistency
        if ( id2 < id1 ) std::swap(id1, id2);
        auto key = std::make_pair(id1, id2);

        currentCollisions.insert(key);

        // Store optional data (could be expanded per-contact point)
        contactTransformCache[key] = { point, normal };

        // Store
        contactEntityCache[key] = std::make_pair(e1, e2);

        interactionCache[key] = interaction;
    }

    void CollisionManager::endFrame() {
        for ( auto& c : currentCollisions ) {
            const auto& extra = contactTransformCache[c];
            const auto& entities = contactEntityCache[c];
            const auto& interaction = interactionCache[c];

            if ( entities.first->getId() == entities.second->getId() ) continue;

            if ( previousCollisions.find(c) == previousCollisions.end() ) {
                CollisionEventData evt(
                    CollisionEventData::Type::Enter,
                    interaction,
                    c.first, c.second,
                    extra.first, extra.second,
                    const_cast<Entity*>(entities.first),
                    const_cast<Entity*>(entities.second));
                events.push_back(evt);
            }
            else {
                CollisionEventData evt(
                    CollisionEventData::Type::Stay,
                    interaction,
                    c.first, c.second,
                    extra.first, extra.second,
                    const_cast<Entity*>(entities.first),
                    const_cast<Entity*>(entities.second));
                events.push_back(evt);
            }
        }

        // Exit collisions
        for ( auto& c : previousCollisions ) {
            if ( currentCollisions.find(c) == currentCollisions.end() ) {
                const Entity* e1 = nullptr;
                const Entity* e2 = nullptr;

                auto entIt = contactEntityCache.find(c);

                if ( entIt != contactEntityCache.end() ) {
                    e1 = entIt->second.first;
                    e2 = entIt->second.second;
                }

                CollisionEventData evt(
                    CollisionEventData::Type::Exit,
                    interactionCache[c],
                    c.first, c.second,
                    const_cast<Entity*>(e1),
                    const_cast<Entity*>(e2));
                events.push_back(evt);

                // Cleanup stale entity pair 
                contactEntityCache.erase(c);
                contactTransformCache.erase(c);
                interactionCache.erase(c);
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
