#include "TestDefinitions.h"

namespace LaurelEye {

    void eventTest() {
        EventManager manager;
        Entity e1, e2;
        LaurelEye::Physics::CollisionEventData ced = {
            LaurelEye::Physics::CollisionEventData::Type::Enter,
            e1.getId(),
            e2.getId(),
            Vector3(1,1,1),
            Vector3(0,1,0),
            &e1,
            &e2
            };

        bool wasCalled = false;
        bool wasCalledTwice = false;

        // 1️⃣ Test: AddListener + BroadcastEvent
        uint32_t listenerId = manager.addListener<CollisionEvent>(
            [&](const CollisionEvent& event) {
                wasCalled = true;
                std::cout << event.GetData() << std::endl;
                //assert(event.collisionData.entityARef == &e1);
                //assert(event.collisionData.entityBRef == &e2);
            });

        CollisionEvent event(ced);
        manager.broadcastEvent(event);
        assert(wasCalled && "Listener should be called on BroadcastEvent");

        // 2️⃣ Test: Multiple Listeners
        int callCount = 0;
        uint32_t listenerId2 = manager.addListener<CollisionEvent>(
            [&](const CollisionEvent&) { callCount++; });
        uint32_t listenerId3 = manager.addListener<CollisionEvent>(
            [&](const CollisionEvent&) { callCount++; });

        manager.broadcastEvent(event);
        assert(callCount == 2 && "All listeners should be called");

        // 3️⃣ Test: RemoveListener
        manager.removeListener<CollisionEvent>(listenerId);
        wasCalled = false;
        callCount = 0;
        manager.broadcastEvent(event);
        // Should not call removed listener (callCount stays same)
        assert(callCount == 2 && "Removed listener should not be called");
        assert(!wasCalled && "Removed listener shouldn't trigger again");

        // 4️⃣ Test: Different event type isolation
        struct DummyEvent : public IEvent {};
        bool dummyCalled = false;
        manager.addListener<DummyEvent>(
            [&](const DummyEvent&) { dummyCalled = true; });

        DummyEvent dummy;
        manager.broadcastEvent(dummy);
        assert(dummyCalled && "DummyEvent listener should trigger");

        // Ensure CollisionEvent listener not affected
        callCount = 0;
        manager.broadcastEvent(event);
        assert(callCount == 2 && "DummyEvent should not affect CollisionEvent listeners");

        std::cout << "All EventManager tests passed successfully!\n";
    }
} // namespace LaurelEye
