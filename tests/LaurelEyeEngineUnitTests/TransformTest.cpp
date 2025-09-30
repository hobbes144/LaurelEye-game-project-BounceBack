#include "TestDefinitions.h"

namespace LaurelEye {

    void transformTest() {
        // NOTE: This test assumes the existence of:
        //  - Transform(Vector3 position, Quaternion rotation, Vector3 scale)
        //  - Transform::getPosition(), getRotation(), getScaling()
        //  - Transform operator*(const Transform&) to compose transforms
        //  - Transform::getInverseLocalMatrix() used in TransformSystem
        //  - Vector3 { x,y,z } and Quaternion { x,y,z,w }
        //
        // These tests focus on TransformComponent API and consistency
        // of local/world setters and parent/child linking.

        // Create three transform components: root, childLocal, childWorld
        TransformComponent rootComp;
        TransformComponent childLocalComp;
        TransformComponent childWorldComp;

        // Basic API sanity checks: defaults
        assert(rootComp.getParent() == nullptr);
        assert(rootComp.getChildren().empty());
        assert(rootComp.getIsLocalSpace()); // default is local-space

        // Simple transforms for testing
        // Construct small helper values. Replace Vec/Quat constructors with actual types.
        Vector3 rootPos{10.0f, 0.0f, 0.0f};
        Quaternion rootRot{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 rootScale{1.0f, 1.0f, 1.0f};

        Vector3 localPos{1.0f, 2.0f, 3.0f};
        Quaternion localRot{0.0f, 0.0f, 0.0f, 1.0f};
        Vector3 localScale{2.0f, 2.0f, 2.0f};

        Vector3 worldPos{50.0f, -3.0f, 7.0f};
        Quaternion worldRot{0.0f, 0.7071068f, 0.0f, 0.7071068f}; // 90 deg about Y
        Vector3 worldScale{0.5f, 0.5f, 0.5f};

        // Create transforms (replace with your Transform constructor)
        Transform tRoot(rootPos, rootRot, rootScale);
        Transform tLocal(localPos, localRot, localScale);
        Transform tWorld(worldPos, worldRot, worldScale);

        // Assign local transform to root and ensure world = local for root
        rootComp.setLocalTransform(tRoot);
        assert(rootComp.getIsLocalSpace());
        assert(rootComp.getLocalTransform().getPosition() == rootPos);
        // For a root with no parent, world should equal local after conversion
        rootComp.setWorldTransform(rootComp.getLocalTransform()); // simulate system behavior for root
        assert(rootComp.getWorldTransform().getPosition() == rootPos);

        // Setup childLocal: parented to root, local-space transform set
        childLocalComp.setParent(&rootComp);
        rootComp.addChild(&childLocalComp);
        childLocalComp.setLocalTransform(tLocal); // marks isLocalSpace = true

        assert(childLocalComp.getParent() == &rootComp);
        assert(!childLocalComp.getChildren().size()); // no children

        // Simulate TransformSystem::updateTransforms for childLocal:
        // If parent exists and child is local, world = parent.world * child.local
        Transform expectedChildWorld = rootComp.getWorldTransform() * childLocalComp.getLocalTransform();
        childLocalComp.setWorldTransform(expectedChildWorld); // simulate what the system would do
        assert(childLocalComp.getWorldTransform().getPosition() == expectedChildWorld.getPosition());
        assert(childLocalComp.getWorldTransform().getRotation() == expectedChildWorld.getRotation());
        assert(childLocalComp.getWorldTransform().getScaling() == expectedChildWorld.getScaling());

        // Setup childWorld: parented to root, but provided in world-space
        childWorldComp.setParent(&rootComp);
        rootComp.addChild(&childWorldComp);
        childWorldComp.setWorldTransform(tWorld); // marks isLocalSpace = false

        // Now simulate TransformSystem's "world -> local" branch:
        // newLocal = parentWorld.getInverseLocalMatrix() * childWorld.getPosition()  (and rotations/scales accordingly)
        // We cannot directly invert matrices here without the Transform implementation details,
        // but we can use the same algorithm used in the system to compute expected local:
        Transform parentWorld = rootComp.getWorldTransform();
        Transform childWorld = childWorldComp.getWorldTransform();

        // Recreate the computation from TransformSystem::updateTransforms
        Transform computedLocal(
            parentWorld.getInverseLocalMatrix() * childWorld.getPosition(),
            childWorld.getRotation() * parentWorld.getRotation().inverse(),
            childWorld.getScaling() * parentWorld.getScaling().reciprocal());

        // Simulate system effect: setLocalTransform(computedLocal) and mark local-space
        childWorldComp.setLocalTransform(computedLocal);
        childWorldComp.setIsLocalSpace(true);

        // Validate that the computed local applied correctly
        assert(childWorldComp.getIsLocalSpace());
        assert(childWorldComp.getLocalTransform().getPosition() == computedLocal.getPosition());
        assert(childWorldComp.getLocalTransform().getRotation() == computedLocal.getRotation());
        assert(childWorldComp.getLocalTransform().getScaling() == computedLocal.getScaling());

        // Basic parent-child consistency: root should have two children
        assert(rootComp.getChildren().size() == 2);

        std::cout << "TransformComponent tests passed\n";
    }

    void transformSystemTest() {
        TransformSystem system;
        system.initialize();

        // Create root and child components
        auto root = std::make_unique<TransformComponent>();
        auto child = std::make_unique<TransformComponent>();

        // Setup hierarchy
        child->setParent(root.get());
        root->addChild(child.get());

        // Set local transforms
        Transform rootLocal({10, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});
        Transform childLocal({5, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});

        root->setLocalTransform(rootLocal);
        child->setLocalTransform(childLocal);

        // Register components with system
        system.registerComponent(root.get());
        system.registerComponent(child.get());

        // Run update
        system.update(0.016f);

        // Validate world transforms
        Transform expectedChildWorld = root->getWorldTransform() * child->getLocalTransform();
        assert(child->getWorldTransform().getPosition() == expectedChildWorld.getPosition());
        assert(child->getWorldTransform().getRotation() == expectedChildWorld.getRotation());
        assert(child->getWorldTransform().getScaling() == expectedChildWorld.getScaling());

        std::cout << "TransformSystem hierarchy update passed\n";

        // Test world-to-local conversion
        Transform worldOverride({100, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});
        child->setWorldTransform(worldOverride); // override with world-space
        child->setIsLocalSpace(false);

        system.update(0.016f); // should convert to local

        Transform expectedLocal(
            root->getWorldTransform().getInverseLocalMatrix() * worldOverride.getPosition(),
            worldOverride.getRotation() * root->getWorldTransform().getRotation().inverse(),
            worldOverride.getScaling() * root->getWorldTransform().getScaling().reciprocal());

        assert(child->getLocalTransform().getPosition() == expectedLocal.getPosition());
        assert(child->getLocalTransform().getRotation() == expectedLocal.getRotation());
        assert(child->getLocalTransform().getScaling() == expectedLocal.getScaling());

        std::cout << "TransformSystem world-to-local conversion passed\n";

        system.shutdown();
    }

    void transformHierarchyTest() {
        TransformSystem system;
        system.initialize();

        // Build a 3-level hierarchy: root -> child -> grandchild
        auto root = std::make_unique<TransformComponent>();
        auto child = std::make_unique<TransformComponent>();
        auto grandChild = std::make_unique<TransformComponent>();

        child->setParent(root.get());
        root->addChild(child.get());

        grandChild->setParent(child.get());
        child->addChild(grandChild.get());

        // Set local transforms
        Transform tRoot({10, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});
        Transform tChild({5, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});
        Transform tGrand({2, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});

        root->setLocalTransform(tRoot);
        child->setLocalTransform(tChild);
        grandChild->setLocalTransform(tGrand);

        // Register with system
        system.registerComponent(root.get());
        system.registerComponent(grandChild.get()); // Added out of order, still should work.
        system.registerComponent(child.get());
        
        // Initial update should propagate world transforms
        system.update(0.016f);

        // Expected results
        Transform expectedChildWorld = root->getWorldTransform() * child->getLocalTransform();
        Transform expectedGrandWorld = expectedChildWorld * grandChild->getLocalTransform();

        assert(child->getWorldTransform().getPosition() == expectedChildWorld.getPosition());
        assert(grandChild->getWorldTransform().getPosition() == expectedGrandWorld.getPosition());

        // --- Dirty flag propagation test ---
        // Change root, children should be marked dirty
        root->setLocalTransform(Transform({20, 0, 0}, {0, 0, 0, 1}, {1, 1, 1}));
        assert(root->isDirty());
        assert(child->isDirty());
        assert(grandChild->isDirty());

        system.update(0.016f);

        // Recompute expectations
        expectedChildWorld = root->getWorldTransform() * child->getLocalTransform();
        expectedGrandWorld = expectedChildWorld * grandChild->getLocalTransform();

        assert(child->getWorldTransform().getPosition() == expectedChildWorld.getPosition());
        assert(grandChild->getWorldTransform().getPosition() == expectedGrandWorld.getPosition());

        // --- No redundant updates test ---
        // After update, dirty flags should be clear
        assert(!root->isDirty());
        assert(!child->isDirty());
        assert(!grandChild->isDirty());

        // Updating again without changes should not mark dirty
        system.update(0.016f);
        assert(!root->isDirty());
        assert(!child->isDirty());
        assert(!grandChild->isDirty());

        // --- World-to-local override test ---
        Transform newWorld({100, 0, 0}, {0, 0, 0, 1}, {1, 1, 1});
        grandChild->setWorldTransform(newWorld);
        grandChild->setIsLocalSpace(false); // explicitly world override

        system.update(0.016f);

        Transform expectedLocal(
            child->getWorldTransform().getInverseLocalMatrix() * newWorld.getPosition(),
            newWorld.getRotation() * child->getWorldTransform().getRotation().inverse(),
            newWorld.getScaling() * child->getWorldTransform().getScaling().reciprocal());

        assert(grandChild->getLocalTransform().getPosition() == expectedLocal.getPosition());

        std::cout << "TransformSystem hierarchy + dirty flag test passed\n";

        system.shutdown();
    }
} // namespace LaurelEye
