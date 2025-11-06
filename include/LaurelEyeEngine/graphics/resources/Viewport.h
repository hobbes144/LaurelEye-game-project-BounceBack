#pragma once

#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"

namespace LaurelEye::Graphics {

    // TODO: Move all Viewport related stuff to using this.
    struct Viewport {
        int x;
        int y;
        SizeRegistry size;

        Viewport() : x(0), y(0), size() {};
        Viewport(int _x, int _y, int _width, int _height) : x(_x), y(_y), size(_width, _height) {};

        Viewport(int _x, int _y, const SizeRegistry& _size) : x(_x), y(_y), size(_size) {};

        bool operator==(const Viewport& other) const {
            return x == other.x && y == other.y &&
                   size == other.size;
        }
    };

} // namespace LaurelEye::Graphics
