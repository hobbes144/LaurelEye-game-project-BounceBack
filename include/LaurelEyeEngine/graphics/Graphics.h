/// @file   Graphics.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-15-2025
/// @brief Graphics backend enum header
///
/// This is to be included in all files that require the backend enum.
/// This allows us to make sure we only specify the existence of all the
/// backends in one place.

#pragma once

// #include <glad/glad.h>

namespace LaurelEye::Graphics {
    /// @brief Represents Graphics Backend options (currently only OpenGL)
    enum class GraphicsBackend {
        OpenGL
    };
}
