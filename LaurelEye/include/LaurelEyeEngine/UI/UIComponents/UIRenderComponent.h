///
/// @file   UIRenderComponent.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   01-15-2026
/// @brief  
/// 
///

#pragma once

#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::UI {

    class UIRenderComponent : public Graphics::IRenderableComponent {

    public:
        UIRenderComponent()
            : Graphics::IRenderableComponent(Graphics::RenderComponentType::UIRenderable, {}) {}

        //Binding Transform
        void BindTransform() override;
        const UITransformComponent* GetBoundTransform() const;

        //Colro
        void SetColor(Vector4 c);
        const Vector4& GetColor() const;

        //Transparency
        void SetTransparency(float t);
        float GetTransparency() const;

        //Layer
        int GetLayer() const;
        void SetLayer(int l);

        //Visibity
        void SetVisible();
        void SetInvisible();
        void ToggleVisibility();
        bool IsVisible() const;

        /*
        //Texture
        void SetUseTexture(bool ut);
        void EnableTexture();
        void DisableTexture();
        void ToggleUseTexture();
        bool IsTextureEnabled() const;
        */

    private:
        UITransformComponent* uiTransComp{nullptr};

        Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        float transparency = 1.0;

        unsigned int layer = 0;
        unsigned int zOrder = 0;

        bool visible = true;
        //bool usingTexture = false;

    };

}
