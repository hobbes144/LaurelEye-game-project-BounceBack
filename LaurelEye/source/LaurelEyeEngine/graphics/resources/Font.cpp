#include "LaurelEyeEngine/graphics/resources/Font.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include <algorithm> // std::max
#include <cstring>   // memset
#include <iostream>
#include <glad/glad.h>

namespace LaurelEye::Graphics {

    Font::Font(const std::string& name,
               const std::string& path,
               FT_Library ftLibrary,
               FontManager* fontManager)
        : fontName(name), filePath(path), fontManager(fontManager) {
        auto fullpath = LaurelEye::IO::Paths().assetsDir / LaurelEye::IO::fs::path(path);
        std::string utf8 = fullpath.lexically_normal().string(); // normalize separators

        assert(ftLibrary != nullptr);
        // Load FreeType face
        if ( FT_New_Face(ftLibrary, utf8.c_str(), 0, &ftFace) ) {
            std::cerr << "[Font] Failed to load font face: " << utf8 << "\n";
            valid = false;
            return;
        }

        // Fixed pixel size for atlas generation (e.g., 32px)
        FT_Set_Pixel_Sizes(ftFace, 0, 32);

        // Create HarfBuzz objects
        hbBlob = hb_blob_create_from_file(utf8.c_str()); // ← use full path
        if ( !hbBlob || hb_blob_get_length(hbBlob) == 0 ) {
            std::cerr << "[Font] Failed to create HarfBuzz blob: " << utf8 << "\n";
            valid = false;
            return;
        }

        hbFace = hb_face_create(hbBlob, 0);
        hbFont = hb_font_create(hbFace);
        hb_ft_font_set_funcs(hbFont);

        // Init atlas pixels (R8)
        atlasPixels.resize(static_cast<size_t>(atlasWidth) * static_cast<size_t>(atlasHeight));
        std::memset(atlasPixels.data(), 0, atlasPixels.size());

        // Build initial atlas (ASCII range only)
        buildInitialAtlas();

        valid = true;
    }

    Font::~Font() {
        if ( hbFont ) hb_font_destroy(hbFont);
        if ( hbFace ) hb_face_destroy(hbFace);
        if ( hbBlob ) hb_blob_destroy(hbBlob);
        if ( ftFace ) FT_Done_Face(ftFace);
        // atlasTexture is managed by RenderResources; freed there if needed
    }

    const GlyphInfo* Font::getGlyph(uint32_t glyphIndex) const {
        auto it = glyphs.find(glyphIndex);
        if ( it == glyphs.end() )
            return nullptr;
        return &it->second;
    }

    void Font::buildInitialAtlas() {
        // Preload ASCII printable range: 32–126
        for ( uint32_t c = 32; c < 127; ++c ) {
            uint32_t glyphIndex = FT_Get_Char_Index(ftFace, c);
            if ( glyphIndex != 0 ) {
                loadGlyph(glyphIndex);
            }
        }

        // Upload once (no dynamic updates later)
        uploadAtlasTexture();
    }

    void Font::loadGlyph(uint32_t glyphIndex) {
        // Already loaded?
        if ( glyphs.find(glyphIndex) != glyphs.end() )
            return;

        if ( FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_RENDER) ) {
            std::cerr << "[Font] Failed to load glyph index: " << glyphIndex << "\n";
            return;
        }

        FT_GlyphSlot g = ftFace->glyph;
        FT_Bitmap& bmp = g->bitmap;

        if ( bmp.width == 0 || bmp.rows == 0 ) {
            // Empty glyph (space, etc.)
            GlyphInfo info{};
            info.u0 = info.v0 = info.u1 = info.v1 = 0.0f;
            info.width = info.height = 0;
            info.bearingX = g->bitmap_left;
            info.bearingY = g->bitmap_top;
            info.advance = g->advance.x >> 6;
            glyphs[glyphIndex] = info;
            return;
        }

        packGlyphIntoAtlas(bmp, glyphIndex);
        // NOTE: no upload here; atlas is uploaded once after all ASCII glyphs are packed
    }

    void Font::packGlyphIntoAtlas(FT_Bitmap& bmp, uint32_t glyphIndex) {
        int gw = bmp.width;
        int gh = bmp.rows;

        // New row if needed
        if ( penX + gw >= atlasWidth ) {
            penX = 0;
            penY += rowHeight;
            rowHeight = 0;
        }

        // Check atlas overflow (should not happen for ASCII + 1024x1024)
        if ( penY + gh >= atlasHeight ) {
            std::cerr << "[Font] Atlas overflow for font: " << fontName << "\n";
            return;
        }

        // Copy bitmap into atlasPixels at (penX, penY)
        for ( int y = 0; y < gh; ++y ) {
            int dstY = penY + y;
            int dstX = penX;
            unsigned char* dstRow = &atlasPixels[dstY * atlasWidth + dstX];

            unsigned char* srcRow = bmp.buffer + y * bmp.pitch;
            std::memcpy(dstRow, srcRow, gw);
        }

        // Compute UVs
        // Flip V so UV (0,0) = bottom-left matches OpenGL convention
        float u0 = static_cast<float>(penX) / static_cast<float>(atlasWidth);
        float v0 = static_cast<float>(penY) / static_cast<float>(atlasHeight);
        float u1 = static_cast<float>(penX + gw) / static_cast<float>(atlasWidth);
        float v1 = static_cast<float>(penY + gh) / static_cast<float>(atlasHeight);

        GlyphInfo info{};
        info.u0 = u0;
        info.v0 = v0;
        info.u1 = u1;
        info.v1 = v1;
        info.width = gw;
        info.height = gh;
        info.bearingX = ftFace->glyph->bitmap_left;
        info.bearingY = ftFace->glyph->bitmap_top;
        info.advance = ftFace->glyph->advance.x >> 6;

        glyphs[glyphIndex] = info;

        // Advance packing cursor
        penX += gw;
        rowHeight = std::max(rowHeight, gh);
    }

    void Font::uploadAtlasTexture() {
        RenderResources* rr = fontManager->getRenderResources();

        if ( !isValidTexture(atlasTexture) ) {
            TextureDesc desc;
            desc.debugName = "FontAtlas_" + fontName;
            desc.type = TextureType::Texture2D;
            desc.format = TextureFormat::R8;
            desc.width = static_cast<uint32_t>(atlasWidth);
            desc.height = static_cast<uint32_t>(atlasHeight);
            desc.mipMode = TextureMipMode::None;

            atlasTexture = rr->createTexture(
                fontName,          // name
                desc,              // description
                "Font",            // importer tag
                atlasPixels.data() // pixel data
            );

        }
        // No update path: atlas is built once and never modified
    }

    std::vector<ShapedGlyph> Font::shapeText(const std::string& text, float fontSize) {
        std::vector<ShapedGlyph> result;
        if ( !valid || text.empty() )
            return result;

        // We keep atlas at fixed size (e.g., 32px), but still set scale for metrics
        FT_Set_Pixel_Sizes(ftFace, 0, static_cast<FT_UInt>(fontSize));

        int hbScale = static_cast<int>(fontSize * 64.0f);
        hb_font_set_scale(hbFont, hbScale, hbScale);

        hb_buffer_t* buf = hb_buffer_create();
        hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);
        hb_buffer_guess_segment_properties(buf);

        hb_shape(hbFont, buf, nullptr, 0);

        unsigned int glyphCount = 0;
        hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buf, &glyphCount);
        hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buf, &glyphCount);

        result.reserve(glyphCount);

        float x = 0.0f;
        float y = 0.0f;

        for ( unsigned int i = 0; i < glyphCount; ++i ) {
            uint32_t glyphIndex = glyphInfo[i].codepoint;

            // We do NOT load glyphs here; atlas is fixed (ASCII only)
            auto it = glyphs.find(glyphIndex);
            if ( it == glyphs.end() )
                continue; // skip unsupported characters

            GlyphInfo* info = &it->second;

            float xOffset = glyphPos[i].x_offset / 64.0f;
            float yOffset = glyphPos[i].y_offset / 64.0f;
            float xAdvance = glyphPos[i].x_advance / 64.0f;
            float yAdvance = glyphPos[i].y_advance / 64.0f;

            ShapedGlyph sg;
            sg.glyphIndex = glyphIndex;
            sg.x = x + xOffset;
            sg.y = y + yOffset;
            sg.info = info;

            result.push_back(sg);

            x += xAdvance;
            y += yAdvance;
        }

        hb_buffer_destroy(buf);
        return result;
    }

} // namespace LaurelEye::Graphics
