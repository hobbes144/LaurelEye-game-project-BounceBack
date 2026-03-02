/// @file   Material.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Material

#include "LaurelEyeEngine/graphics/resources/Material.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Graphics {

    std::unordered_map<std::string, std::shared_ptr<Material>> Material::basicMaterials;

    void Material::setTexture(const std::string& name, TextureHandle handle) {
        if ( !isValidTexture(handle) ) {
            textureBindings.erase(name);
        }
        else {
            textureBindings[name] = handle;
        }
    }

    TextureHandle Material::getTexture(const std::string& name) const {
        auto it = textureBindings.find(name);
        if ( it == textureBindings.end() ) return InvalidTexture;
        return it->second;
    }

    void Material::removeTexture(const std::string& name) {
        textureBindings.erase(name);
    }

    void Material::apply(std::shared_ptr<Shader> shader) const {
        if ( !shader ) return;

        // Ensure the shader program is active so glUniform calls take effect.
        shader->use();

        /* Todo: this is VERY inefficient. We don't need to be using variant
         * and doing TWO loops over the same variant data is really bad for
         * performance. Please fix this when you get time.
         *
         * Also a nightmare to manage. I forgot to add Vector2 to one and
         * took half an hour to realise.
         */

        std::vector<std::pair<std::string, TextureHandle>> texturesToBind;

        for ( const auto& [name, value] : properties ) {
            if ( auto item = std::get_if<unsigned int>(&value) ) {
                shader->setUInt(name, *item);
            }
            else if ( auto item = std::get_if<int>(&value) ) {
                shader->setInt(name, *item);
            }
            else if ( auto item = std::get_if<float>(&value) ) {
                shader->setFloat(name, *item);
            }
            else if ( auto item = std::get_if<Vector2>(&value) ) {
                shader->setVec2(name, (*item)[0], (*item)[1]);
            }
            else if ( auto item = std::get_if<Vector3>(&value) ) {
                shader->setVec3(name, *item);
            }
            else if ( auto item = std::get_if<Vector4>(&value) ) {
                shader->setVec4(name, (*item)[0], (*item)[1], (*item)[2], (*item)[3]);
            }
            else if ( auto item = std::get_if<Matrix4>(&value) ) {
                shader->setMat4(name, *item);
            }
        }

        // Upload temporary properties
        for ( const auto& [name, value] : tempProperties ) {
            if ( auto item = std::get_if<unsigned int>(&value) ) {
                shader->setUInt(name, *item);
            }
            else if ( auto item = std::get_if<int>(&value) ) {
                shader->setInt(name, *item);
            }
            else if ( auto item = std::get_if<float>(&value) ) {
                shader->setFloat(name, *item);
            }
            else if ( auto item = std::get_if<VectorTemplated<float, 2>>(&value) ) {
                shader->setVec2(name, (*item)[0], (*item)[1]);
            }
            else if ( auto item = std::get_if<Vector3>(&value) ) {
                shader->setVec3(name, *item);
            }
            else if ( auto item = std::get_if<Vector4>(&value) ) {
                shader->setVec4(name, (*item)[0], (*item)[1], (*item)[2], (*item)[3]);
            }
            else if ( auto item = std::get_if<Matrix4>(&value) ) {
                shader->setMat4(name, *item);
            }
        }

        // Ensure shader has deterministic texture-related uniforms set even if the
        // property map doesn't contain them. This prevents uniform carry-over between
        // draws when the same shader program is used for multiple objects.
        // shader->setInt("useTexture", useTexture ? 1 : 0);
        // shader->setVec2("mainTextureScale", mainTextureScale[0], mainTextureScale[1]);
        // Bind textures previously set via setTexture(...)
        // Start from texture unit 0 and increment for each bound texture.
        unsigned int textureUnit = 0;
        // GLint maxUnits = 0;
        // glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        for ( const auto& [name, textureID] : textureBindings ) {
            if ( textureID != 0 ) {
                // if ( static_cast<GLint>(textureUnit) >= maxUnits ) {
                //     std::cout << "Peepee poopoo" << std::endl;
                //     continue;
                //
                // }
                shader->bindTexture(textureUnit, name, textureID);
                ++textureUnit;
            }
        }
    }

    void Material::unbindTextures(std::shared_ptr<Shader> shader) const {
        unsigned int textureUnit = 0;
        // GLint maxUnits = 0;
        // glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        for ( const auto& [name, textureID] : textureBindings ) {
            if ( textureID != 0 ) {
                // if ( static_cast<GLint>(textureUnit) >= maxUnits ) {
                //     std::cout << "Peepee poopoo" << std::endl;
                //     continue;
                //
                // }
                shader->unbindTexture(textureUnit);
                ++textureUnit;
            }
        }
    }

} // namespace LaurelEye::Graphics
