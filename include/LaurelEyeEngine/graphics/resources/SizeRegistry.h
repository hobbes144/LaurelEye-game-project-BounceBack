#pragma once

#include <cstdint>

namespace LaurelEye::Graphics {

    struct SizeRegistry {
        uint32_t width;
        uint32_t height;



        // SizeRegistry() : width(), height() {}
        // // Copy constructor (important for proper object copying)
        // SizeRegistry(const SizeRegistry& other) : width(other.width), height(other.height) {}
        //
        // // Assignment operator overload
        // SizeRegistry& operator=(const SizeRegistry& other) {
        //     if ( this == &other ) { // Handle self-assignment
        //         return *this;
        //     }
        //
        //     width = other.width;
        //     height = other.height;
        //
        //     return *this; // Return reference to current object
        // }

        inline bool operator==(const SizeRegistry& other) const {
            return width == other.width && height == other.height;
        }
    };
} // namespace LaurelEye::Graphics
