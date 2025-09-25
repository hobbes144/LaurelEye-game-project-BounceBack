#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye {

    void Entity::updateTransforms() {
        if ( parent ) {
            if ( isLocalSpace ) {
                worldTransform = parent->getWorldTransform() * localTransform;
            }
            else {
                Transform parentWorldTransform = parent->getWorldTransform();
                localTransform.setPosition(parentWorldTransform.getInverseLocalMatrix() * worldTransform.getPosition());
                localTransform.setRotation(worldTransform.getRotation() * parentWorldTransform.getRotation().inverse());
                localTransform.setScaling(worldTransform.getScaling() * parentWorldTransform.getScaling().reciprocal());
                isLocalSpace = true;
            }
        }
        else { // Allow for root entities
            if ( isLocalSpace )
                worldTransform = localTransform;
            else {
                localTransform = worldTransform;
                isLocalSpace = true;
            }
        }
    }

    void Entity::worldToLocalSpace() {
        if ( parent == nullptr ) {
            // Root entity: local == world
            localTransform = worldTransform;
            isLocalSpace = true;
            return;
        }
        Transform parentWorldTransform = parent->getWorldTransform();
        localTransform.setPosition(parentWorldTransform.getInverseLocalMatrix() * worldTransform.getPosition());
        localTransform.setRotation(worldTransform.getRotation() * parentWorldTransform.getRotation().inverse());
        localTransform.setScaling(worldTransform.getScaling() * parentWorldTransform.getScaling().reciprocal());
        isLocalSpace = true;
    }

    void Entity::localToWorldSpace() {
        if ( parent == nullptr ) {
            // Root entity: world == local
            worldTransform = localTransform;
            return;
        }

        worldTransform = (parent->getWorldTransform() * localTransform);
    }

    void Entity::addChild(std::unique_ptr<Entity> child) {
        if ( !child ) return;
        child->parent = this;
        children.push_back(std::move(child));
    }

    void Entity::removeChild(Entity* child) {
        auto it = std::find_if(children.begin(), children.end(),
                               [&](const std::unique_ptr<Entity>& c) { return c.get() == child; });
        if ( it != children.end() ) {
            (*it)->parent = nullptr; // clear parent
            children.erase(it);
        }
    }

    Entity* Entity::findChildByName(const std::string& name) {
        std::stack<Entity*> stack;
        stack.push(this);

        while ( !stack.empty() ) {
            Entity* current = stack.top();
            stack.pop();

            if ( current->name == name ) {
                return current;
            }

            for ( auto it = current->children.rbegin(); it != current->children.rend(); ++it ) {
                stack.push(it->get());
            }
        }

        return nullptr;
    }

    // TODO - TBD if needed
    void Entity::initialize() {
        std::cout << "Initializing entity" << std::endl;
        // initialize components
        for ( auto& c : components ) c->initialize();
        // initialize children
        for ( auto& child : children ) child->initialize();
    }

    // TODO - TBD if needed. I state this because if we have Transform
    // as a Component/System, no need to even update transforms here. All data
    // and systems
    void Entity::update(float deltaTime) {
        std::cout << "Updating entity: " << name << std::endl;
        updateTransforms();

        // Update components - Potentially remove this
        for ( auto& c : components ) {
            c->update(deltaTime);
        }

        // Update all children
        for (auto& child : children) {
            if ( child && child->activeInHierarchy ) {
                child->update(deltaTime);
            } 
        }
    }
    // TODO - TBD if needed
    void Entity::shutdown() {
        std::cout << "Shutting down: " << name << std::endl;
        for ( auto& c : components ) c->shutdown();
        for ( auto& child : children ) {
            child->shutdown();
        }
    }

    void Entity::addTag(std::string tagToAdd) {
        tags.insert(tagToAdd);
    }

    bool Entity::compareTag(std::string tagToCompare) {
        return tags.find(tagToCompare) != tags.end();
    }
} // namespace LaurelEye
