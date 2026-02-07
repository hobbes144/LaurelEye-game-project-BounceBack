///
/// @file   UIMaterial.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   01-27-2026
/// @brief  
/// 
///

#pragma once

#include "LaurelEyeEngine\graphics\resources\Material.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::UI {

    class UIMaterial : Graphics::Material {

    public:
        Vector4 color = {1, 0, 1, 1};

        void apply(std::shared_ptr<Shader> shader) const override {
            shader->setVec4("u_Color", color);
        }

    };

}
