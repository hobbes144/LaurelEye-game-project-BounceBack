/// @file   Material.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Material

#include "LaurelEyeEngine/graphics/resources/Material.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {

    std::unordered_map<std::string, std::shared_ptr<Material>> Material::basicMaterials;

    void Material::apply(std::shared_ptr<Shader> shader) const {

        /* Todo: this is VERY inefficient. We don't need to be using variant
         * and doing TWO loops over the same variant data is really bad for
         * performance. Please fix this when you get time.
         *
         * Also a nightmare to manage. I forgot to add Vector2 to one and
         * took half an hour to realise.
         */

        //std::vector<std::pair<std::string, TextureManager::TextureID>> texturesToBind;

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
            else if ( auto item = std::get_if<VectorTemplated<float, 2>>(&value) ) {
                shader->setVec2(name, (*item)[0], (*item)[1]);
            }
            else if ( auto item = std::get_if<Vector3>(&value) ) {
                shader->setVec3(name, *item);
            }
            else if ( auto item = std::get_if<Matrix4>(&value) ) {
                shader->setMat4(name, *item);
            }
        }

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
            else if ( auto item = std::get_if<Matrix4>(&value) ) {
                shader->setMat4(name, *item);
            }
        }

        unsigned int textureUnit = 10;
        /*
        for ( const auto& [name, textureID] : texturesToBind ) {
            shader->bindTexture(textureUnit, name, textureID);
            ++textureUnit;
        }
        */
    }

} // namespace LaurelEye::Graphics
