#include "LaurelEyeEngine/memory/MemoryManager.h"
#include <cassert>
#include <iostream>

namespace LaurelEye {
    void memoryTest() {
        LaurelEye::MemoryManager mem(2);

        std::cout << "Initial pool size: " << mem.getPoolSize()
                  << " Free: " << mem.getFreeSize() << std::endl;
        assert(mem.getPoolSize() == 2);
        assert(mem.getFreeSize() == 2);

        // Allocate one entity
        LaurelEye::Entity* e1 = mem.allocateMemory();
        assert(e1 != nullptr);
        std::cout << "Allocated entity 1. Free=" << mem.getFreeSize() << std::endl;
        assert(mem.getFreeSize() == 1);

        // Allocate another entity
        LaurelEye::Entity* e2 = mem.allocateMemory();
        assert(e2 != nullptr);
        std::cout << "Allocated entity 2. Free=" << mem.getFreeSize() << std::endl;
        assert(mem.getFreeSize() == 0);

        // Allocating one more should trigger pool growth
        LaurelEye::Entity* e3 = mem.allocateMemory();
        assert(e3 != nullptr);
        std::cout << "Allocated entity 3 (after reallocation). Pool="
                  << mem.getPoolSize() << " Free=" << mem.getFreeSize() << std::endl;
        assert(mem.getPoolSize() == 4);
        assert(mem.getFreeSize() == 1); // 4 total, 3 in use, 1 free

        // Deallocate e2
        mem.deallocateMemory(e2);
        std::cout << "Deallocated entity 2. Free=" << mem.getFreeSize() << std::endl;
        assert(mem.getFreeSize() == 2);

        // Reuse freed slot
        LaurelEye::Entity* e4 = mem.allocateMemory();
        assert(e4 != nullptr);
        std::cout << "Reallocated freed slot. Free=" << mem.getFreeSize() << std::endl;

        // Validate pool state
        assert(mem.getPoolSize() == 4);
        assert(mem.getFreeSize() == 1);

        // Shutdown clears everything
        mem.shutdown();
        std::cout << "After shutdown: Pool=" << mem.getPoolSize()
                  << " Free=" << mem.getFreeSize() << std::endl;
        assert(mem.getPoolSize() == 0);
        assert(mem.getFreeSize() == 0);

        std::cout << "All MemoryManager tests passed." << std::endl;
    }
} // namespace LaurelEye
