/// @file    MemoryManager.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    9-30-25
/// @brief   File containing the memory manager implementation for the LaurelEyeEngine
/// Initializes a pool of unique entity pointers and creates raw pointers in the free list
/// to "give out" to scenes when creating entities
/// 
#pragma once

#include "LaurelEyeEngine/ecs/Entity.h"
#include <deque>
#include <memory>
#include <list>

namespace LaurelEye {

    class MemoryManager {
    public:
        explicit MemoryManager(std::size_t initialPoolSize = 1000);

        void initialize() {}

        Entity* allocateMemory(std::string name);
        void deallocateMemory(Entity* entity);

        void shutdown();

        std::size_t getPoolSize() const { return memoryPool.size(); }
        std::size_t getFreeSize() const { return freeList.size(); }
    private:
        std::vector<std::unique_ptr<Entity>> memoryPool;
        std::list<Entity*> freeList;

        void reallocateMemory();
    };

} // namespace LaurelEye
