/// @file   FontManager.h
/// @author Martin Chow
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @date   10-08-2025
/// @brief  Centralized manager responsible for loading, caching, and unloading Front for texture.
///

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb-ft.h>
#include <hb.h>

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Graphics {

    class Font;

    class FontManager {
    public:
        /// @brief Retrieve global FontManager instance.
        static FontManager& getInstance() {
            static FontManager instance;
            return instance;
        }

        /// @brief Initializes FreeType library and sets RenderResources dependency.
        ///
        /// Must be called once at engine startup.
        bool initialize(RenderResources* rr) {
            renderResources = rr;

            if ( FT_Init_FreeType(&ftLibrary) ) {
                ftLibrary = nullptr;
                return false;
            }
            return true;
        }

        /// @brief Shutdown FreeType and unload all fonts.
        void shutdown() {
            unloadFonts();

            if ( ftLibrary ) {
                FT_Done_FreeType(ftLibrary);
                ftLibrary = nullptr;
            }

            renderResources = nullptr;
        }

        /// @brief Loads a font file or returns cached instance.
        std::shared_ptr<Font> loadFont(const std::string& name, const std::string& filePath);

        /// @brief Retrieve a previously loaded font by name.
        std::shared_ptr<Font> getFont(const std::string& name);

        /// @brief Unload a specific font instance.
        void unloadFont(std::shared_ptr<Font> font);

        /// @brief Unload a font by its logical name.
        void unloadFont(const std::string& name);

        /// @brief Unload all fonts.
        void unloadFonts();

        /// @brief Access FreeType library handle.
        FT_Library getFTLibrary() const { return ftLibrary; }

        RenderResources* getRenderResources() const { return renderResources; }

    private:
        /// @brief Private constructor for singleton.
        FontManager() = default;

        /// @brief Deleted copy/move to enforce singleton.
        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;

        RenderResources* renderResources = nullptr;
        FT_Library ftLibrary = nullptr;

        std::unordered_map<std::string, std::shared_ptr<Font>> loadedFonts;
    };

} // namespace LaurelEye::Graphics
