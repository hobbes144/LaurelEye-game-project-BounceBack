#include "LaurelEyeEngine/UI/UILayoutSystem.h"
#include "LaurelEyeEngine/window/WindowManager.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"

namespace LaurelEye::UI {

    void UILayoutSystem::initialize() {

        for ( auto* comp : components ) {
            auto* ui = static_cast<UITransformComponent*>(comp);
            ui->MarkDirty();
        }

        auto rs = context->getService<Graphics::RenderSystem>();
        rs->registerResizeCallback(0, [this](const Graphics::SizeRegistry& size) {

            storedWindowSize = Vector2(size.width, size.height);

            for ( auto* comp : components ) {
                auto* ui = static_cast<UITransformComponent*>(comp);
                ui->MarkDirty();
            }

            for ( auto* comp : components ) {
                auto* ui = static_cast<UITransformComponent*>(comp);
                Resolve(*ui, storedWindowSize);
            }

        });

    }

    void UILayoutSystem::update(float dt) {

        for ( auto* comp : components ) {
            auto* ui = static_cast<UITransformComponent*>(comp);

            if ( ui->IsDirty() ) {
                auto renderResources = context->getService<Graphics::RenderSystem>()->getRenderResources();
                const Graphics::SizeRegistry& size = renderResources->getSurfaceSize(0);

                storedWindowSize = Vector2(size.width, size.height);

                //std::cout << "\t\t[UILayout]-WindowSize: { " << storedWindowSize[0] << ", " << storedWindowSize[1] << " }\n";
                Resolve(*ui, storedWindowSize);
            }
        }

    }

    void UILayoutSystem::shutdown() {

    }

    void UILayoutSystem::registerComponent(const ComponentPtr component) {
        ISystem::registerComponent(component);
    }

    void UILayoutSystem::deregisterComponent(ComponentPtr component) {
        ISystem::deregisterComponent(component);
    }

    void UILayoutSystem::Resolve(UITransformComponent& t, Vector2 screenSize) {
        const auto& transformData = t.GetUITransformData();
        float a0 = transformData.anchorMin[0];
        float a1 = transformData.anchorMin[1];

        Vector2 minAnchorPX = {
            transformData.anchorMin[0] * screenSize[0],
            (transformData.anchorMin[1]) * screenSize[1]
        };

        Vector2 maxAnchorPX = {
            transformData.anchorMax[0] * screenSize[0],
            transformData.anchorMax[1] * screenSize[1]
        };

        Vector2 anchorSize = maxAnchorPX - minAnchorPX;

        Vector2 finalSize = transformData.size;
        if ( transformData.anchorMin != transformData.anchorMax ) {
            finalSize += anchorSize;
        }

        // NOTE: Final Size is halved to accomodate enlarged Square Mesh {-1,-1},{+1,+1}
        //This will present issues when checking for intersection
        //Be Consious of GPU / Engine Space Represention of Rects
        finalSize = 0.5 * finalSize;

        Vector2 anchorPoint = minAnchorPX + anchorSize * transformData.pivot;
        Vector2 center = anchorPoint + transformData.localOffset + ((Vector2(0.5f, 0.5f) - transformData.pivot)*2 * finalSize);

        Rect rect;
        rect.center = center;
        rect.size = finalSize;

        t.SetResolvedRect(rect);

    }

}
