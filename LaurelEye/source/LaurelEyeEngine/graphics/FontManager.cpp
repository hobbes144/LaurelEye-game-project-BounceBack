#include "LaurelEyeEngine/graphics/FontManager.h"
#include "LaurelEyeEngine/graphics/resources/Font.h"

namespace LaurelEye::Graphics {

    std::shared_ptr<Font> FontManager::loadFont(const std::string& name, const std::string& filePath) {
        // Guard: manager not initialized
        if ( !ftLibrary || !renderResources ) {
            // Optionally log here
            return nullptr;
        }

        // Return cached font if already loaded
        if ( auto it = loadedFonts.find(name); it != loadedFonts.end() ) {
            return it->second;
        }

        // Create new font
        auto font = std::make_shared<Font>(name, filePath, ftLibrary, this);
        if ( !font || !font->isValid() ) {
            // Optionally log failure here
            return nullptr;
        }

        loadedFonts.emplace(name, font);
        return font;
    }

    std::shared_ptr<Font> FontManager::getFont(const std::string& name) {
        if ( auto it = loadedFonts.find(name); it != loadedFonts.end() ) {
            return it->second;
        }
        return nullptr;
    }

    void FontManager::unloadFont(std::shared_ptr<Font> font) {
        if ( !font ) {
            return;
        }

        for ( auto it = loadedFonts.begin(); it != loadedFonts.end(); ++it ) {
            if ( it->second == font ) {
                loadedFonts.erase(it);
                return;
            }
        }
    }

    void FontManager::unloadFont(const std::string& name) {
        loadedFonts.erase(name);
    }

    void FontManager::unloadFonts() {
        loadedFonts.clear();
    }

} // namespace LaurelEye::Graphics
