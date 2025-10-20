/// @file    UUIDGenerator.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com), Alex Humphries
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    10-8-25
/// @brief   File containing implementation for generating an id for the LaurelEyeEngine
/// Used for the event system to generate IDs for listeners

#pragma once

#include <atomic>

class UUIDGenerator {
public:
    UUIDGenerator() = delete;
    UUIDGenerator(const UUIDGenerator&) = delete;
    UUIDGenerator& operator=(const UUIDGenerator&) = delete;

    static uint32_t generateUUID() {
        return counter.fetch_add(1, std::memory_order_relaxed);
    }

private:
    static inline std::atomic<uint32_t> counter{1};

};
