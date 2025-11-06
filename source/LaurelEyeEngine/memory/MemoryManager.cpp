#include "LaurelEyeEngine/memory/MemoryManager.h"

namespace LaurelEye {

    MemoryManager::MemoryManager(std::size_t initialPoolSize)
    {
        if ( initialPoolSize == 0 ) initialPoolSize = 1;
        memoryPool.reserve(initialPoolSize);
        for ( std::size_t i = 0; i < initialPoolSize; ++i ) {
            auto e = std::make_unique<Entity>();
            freeList.push_back(e.get());
            memoryPool.push_back(std::move(e));
        }
    }

    Entity* MemoryManager::allocateMemory(std::string name)
    {
        if (freeList.empty()) {
            reallocateMemory();
        }

        Entity* e = freeList.front();
        freeList.pop_front();

        *e = Entity(name);

        return e;
    }

    void MemoryManager::deallocateMemory(Entity* entity)
    {
        if (!entity) return;
        entity->reset();
        freeList.push_back(entity);
    }

    void MemoryManager::reallocateMemory()
    {
        const std::size_t oldSize = memoryPool.size();
        const std::size_t newSize = std::max<std::size_t>(1, oldSize * 2);

        memoryPool.reserve(newSize);
        for ( std::size_t i = oldSize; i < newSize; ++i ) {
            auto e = std::make_unique<Entity>();
            freeList.push_back(e.get());
            memoryPool.push_back(std::move(e));
        }
    }

    void MemoryManager::shutdown()
    {
        freeList.clear();
        memoryPool.clear();
    }

} // namespace LaurelEye
