#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Renderable.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderPropertyComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

#include "LaurelEyeEngine/io/AssetManager.h"


namespace LaurelEye::Scripting {
    void Sol2API_Renderable::setup(sol::state& lua, EngineContext* ctx) {
        setupMesh(lua, ctx);
        setupMaterial(lua, ctx);
        setupIRenderable(lua);
        setupRenderer3D(lua);
        setupRenderer2D(lua);
    }

    void Sol2API_Renderable::setupMesh(sol::state& lua, EngineContext* ctx) {
        // enum for primitives
        lua.new_enum("MeshType",
                     "None", Graphics::Mesh::Type::None,
                     "Square", Graphics::Mesh::Type::Square,
                     "Cube", Graphics::Mesh::Type::Cube,
                     "Sphere", Graphics::Mesh::Type::Sphere);

        // Mesh userdata
        lua.new_usertype<Graphics::Mesh>(
            "Mesh",
            sol::no_constructor,
            "getName", &Graphics::Mesh::getName,
            "getVertexCount", &Graphics::Mesh::getVertexCount,
            "getIndexCount", &Graphics::Mesh::getIndexCount);

        // For mesh loading
        lua["Mesh"] = lua.create_table();
        lua["Mesh"]["load"] = [ctx](const std::string& path) -> std::shared_ptr<Graphics::Mesh> {
            if ( path == "Square" ) return Graphics::Mesh::getShapeMesh(Graphics::Mesh::Type::Square);
            if ( path == "Cube" ) return Graphics::Mesh::getShapeMesh(Graphics::Mesh::Type::Cube);
            if ( path == "Sphere" ) return Graphics::Mesh::getShapeMesh(Graphics::Mesh::Type::Sphere);

            // Use AssetManager first (preferred): expects a registered asset path
            if ( ctx ) {
                auto* am = ctx->getService<IO::AssetManager>();
                if ( am ) {
                    auto asset = am->load(path);
                    if ( asset ) {
                        auto meshAsset = std::dynamic_pointer_cast<IO::MeshAsset>(asset);
                        if ( meshAsset ) {
                            try {
                                return Graphics::Mesh::createMeshFromAsset(meshAsset);
                            }
                            catch ( const std::exception& e ) {
                                LE_ERROR("Scripting", "[Lua Mesh.load] createMeshFromAsset failed: " << e.what());
                                return nullptr;
                            }
                        }
                    }
                }
            }

            // As last resort try direct loader
            try {
                return Graphics::Mesh::loadMesh(path);
            }
            catch ( const std::exception& e ) {
                LE_ERROR("Scripting", "[Lua Mesh.load] failed to load mesh '" << path << "': " << e.what());
                return nullptr;
            }
        };
    }

    void Sol2API_Renderable::setupMaterial(sol::state& lua, EngineContext* ctx) {
        // Bind Material type and convenient setters/getters.
        lua.new_usertype<Graphics::Material>(
            "Material",
            sol::constructors<Graphics::Material()>(),

            // Generic setProperty overloads - explicit lambdas for type-safety.
            "setProperty",
            sol::overload(
                [](Graphics::Material& m, const std::string& name, float v) { m.setProperty<float>(name, v); },
                [](Graphics::Material& m, const std::string& name, int v) { m.setProperty<int>(name, v); },
                [](Graphics::Material& m, const std::string& name, unsigned int v) { m.setProperty<unsigned int>(name, v); },
                [](Graphics::Material& m, const std::string& name, Vector2 v) { m.setProperty<Vector2>(name, v); },
                [](Graphics::Material& m, const std::string& name, Vector3 v) { m.setProperty<Vector3>(name, v); },
                [](Graphics::Material& m, const std::string& name, Vector4 v) { m.setProperty<Vector4>(name, v); },
                [](Graphics::Material& m, const std::string& name, Matrix4 v) { m.setProperty<Matrix4>(name, v); }),

            // Texture helpers
            "setTexture", &Graphics::Material::setTexture,
            "getTexture", &Graphics::Material::getTexture,
            "removeTexture", &Graphics::Material::removeTexture,
            "useTextureFlag", &Graphics::Material::useTextureFlag,
            "setTextureScale", &Graphics::Material::setTextureScale,

            // convenience
            "setDiffuse", [](Graphics::Material& m, float r, float g, float b) { m.setProperty<Vector3>("diffuse", Vector3(r, g, b)); },
            "setSpecular", [](Graphics::Material& m, float r, float g, float b) { m.setProperty<Vector3>("specular", Vector3(r, g, b)); },
            "setShininess", [](Graphics::Material& m, float s) { m.setProperty<float>("shininess", s); });

        // Add a helper: Material.fromImage(path) -> Material*
        // This will use AssetManager to fetch the image asset and then call Material::FromAsset
        /*
        lua["Material"]["fromImage"] = [ctx](const std::string& path) -> std::shared_ptr<Graphics::Material> {
            if ( !ctx ) return nullptr;
            auto* am = ctx->getService<IO::AssetManager>();
            if ( !am ) {
                std::cerr << "[Lua Material.fromImage] AssetManager not found in context\n";
                return nullptr;
            }
            auto asset = am->load(path);
            if ( !asset ) {
                std::cerr << "[Lua Material.fromImage] failed to load asset: " << path << std::endl;
                return nullptr;
            }
            auto mat = std::make_shared<Graphics::Material>(asset);
            return mat;
        };
        */
    }

    void Sol2API_Renderable::setupIRenderable(sol::state& lua) {
        lua.new_usertype<Graphics::IRenderComponent>(
            "IRenderComponent",
            sol::base_classes, sol::bases<LaurelEye::IComponent>(),
            "getRenderID", &Graphics::IRenderComponent::GetRenderID,
            "getTransform", &Graphics::IRenderComponent::GetBoundTransform,
            "getType", &Graphics::IRenderComponent::GetRenderCompType);

        lua.new_usertype<Graphics::IRenderPropertyComponent>(
            "IRenderPropertyComponent",
            sol::base_classes, sol::bases<Graphics::IRenderComponent>());

        lua.new_usertype<Graphics::IRenderableComponent>(
            "IRenderableComponent",
            sol::base_classes, sol::bases<Graphics::IRenderComponent>(),
            // Basic checks and setters we expose to Lua (high-level only)
            "hasMesh", &Graphics::IRenderableComponent::HasMesh,
            "hasMaterial", &Graphics::IRenderableComponent::HasMaterial,
            "setMeshAsset", &Graphics::IRenderableComponent::SetMeshAsset,
            "setImageAsset", &Graphics::IRenderableComponent::SetImageAsset,
            "setMeshPrimitiveType", &Graphics::IRenderableComponent::SetMeshPrimitiveType,
            "getMaterial", &Graphics::IRenderableComponent::GetMaterial,
            "setMaterial", &Graphics::IRenderableComponent::SetMaterial);
    }

    void Sol2API_Renderable::setupRenderer3D(sol::state& lua) {
        lua.new_usertype<Graphics::Renderable3DComponent>(
            "Renderable3DComponent",
            sol::constructors<Graphics::Renderable3DComponent()>(),

            sol::base_classes, sol::bases<Graphics::IRenderableComponent>());
    }

    void Sol2API_Renderable::setupRenderer2D(sol::state& lua) {
        lua.new_usertype<Graphics::Renderable2DComponent>(
            "Renderable2DComponent",
            sol::constructors<Graphics::Renderable2DComponent()>(),

            sol::base_classes, sol::bases<Graphics::IRenderableComponent>());
    }

}// namespace LaurelEye::Scripting
