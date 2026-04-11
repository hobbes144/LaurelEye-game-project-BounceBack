#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb-ft.h>
#include <hb.h>

#include "LaurelEyeEngine/graphics/FontManager.h"
#include "LaurelEyeEngine/io/Paths.h"

namespace LaurelEye::Graphics {

    struct GlyphInfo {
        // UV coordinates in atlas
        float u0, v0;
        float u1, v1;

        // Pixel size of glyph
        int width;
        int height;

        // Bearing (offset from baseline)
        int bearingX;
        int bearingY;

        // Advance to next glyph
        int advance;
    };

    struct ShapedGlyph {
        uint32_t glyphIndex;
        float x; // final positioned x
        float y; // final positioned y
        GlyphInfo* info;
    };

    class Font {
    public:
        Font(const std::string& name,
             const std::string& path,
             FT_Library ftLibrary,
             FontManager* ownerManager);

        ~Font();

        bool isValid() const { return valid; }

        const std::string& getName() const { return fontName; }

        TextureHandle getAtlasTexture() const { return atlasTexture; }

        const GlyphInfo* getGlyph(uint32_t glyphIndex) const;

        /// @brief Shapes UTF‑8 text using HarfBuzz and returns positioned glyphs.
        std::vector<ShapedGlyph> shapeText(const std::string& text, float fontSize);

        /// @brief Ensures glyph is in atlas (lazy load).
        void loadGlyph(uint32_t glyphIndex);

    private:
        void buildInitialAtlas();
        void uploadAtlasTexture();
        void packGlyphIntoAtlas(FT_Bitmap& bmp, uint32_t glyphIndex);

    private:
        std::string fontName;
        std::string filePath;

        bool valid = false;

        // FreeType
        FT_Face ftFace = nullptr;

        // HarfBuzz
        hb_blob_t* hbBlob = nullptr;
        hb_face_t* hbFace = nullptr;
        hb_font_t* hbFont = nullptr;

        // Atlas texture
        TextureHandle atlasTexture = InvalidTexture;
        int atlasWidth = 1024;
        int atlasHeight = 1024;

        // CPU-side atlas bitmap
        std::vector<unsigned char> atlasPixels;

        // Simple atlas packing cursor
        int penX = 0;
        int penY = 0;
        int rowHeight = 0;

        // Glyph metrics + UVs
        std::unordered_map<uint32_t, GlyphInfo> glyphs;
        FontManager* fontManager = nullptr;
    };

} // namespace LaurelEye::Graphics
