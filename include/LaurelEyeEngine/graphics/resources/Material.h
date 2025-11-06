/// @file   Material.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Material class responsible for managing shader properties and material instances.
///
/// The Material class provides functionality for setting, storing, and applying shader
/// parameters such as floats, vectors, and matrices. It also serves as a lightweight
/// factory and caching system for reusable material instances.

#pragma once

#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace LaurelEye::IO {
    class ImageAsset;
}

namespace LaurelEye::Graphics {

    class Shader;

    /// @class Material
    /// @brief Class responsible for managing shader parameters and material properties.
    ///
    /// The Material class allows users to define a set of properties (uniforms)
    /// for shaders. These include numeric types, vectors, and matrices.
    /// It supports both persistent and temporary properties for flexible rendering.
    class Material : public std::enable_shared_from_this<Material> {
    public:
        /// @brief Defines a mapping from property names to their stored values.
        ///
        /// Supported types include:
        /// - `unsigned int`, `int`, `float`
        /// - `VectorTemplated<float, 2>` (e.g., vec2)
        /// - `Vector3`
        /// - `Matrix4`
        using PropertyMap = std::unordered_map<
            std::string,
            std::variant<
                unsigned int, int, float, Vector2, Vector3, DVector4, Matrix4>>;

        /// @brief Default constructor for Material.
        Material() = default;

        /// @brief Virtual destructor for proper cleanup in derived classes.
        virtual ~Material() = default;

        /// @brief Sets a persistent shader property value.
        ///
        /// @tparam T Type of the property value (e.g., float, int, Vector3).
        /// @param name Property name (uniform name in shader).
        /// @param value Property value.
        template <typename T>
        void setProperty(const std::string& name, const T& value) {
            properties[name] = value;
        }

        /// @brief Sets a temporary shader property value (used for one frame or render pass).
        ///
        /// @tparam T Type of the property value (e.g., float, int, Vector3).
        /// @param name Property name (uniform name in shader).
        /// @param value Property value.
        template <typename T>
        void setTempProperty(const std::string& name, const T& value) {
            tempProperties[name] = value;
        }

        /// @brief Clears all temporary properties.
        ///
        /// This is typically called after a render pass to reset transient data.
        void clearTempProperties() {
            tempProperties.clear();
        }

        /// @brief Applies this material’s properties to the specified shader.
        ///
        /// @param shader Shared pointer to the target shader.
        virtual void apply(std::shared_ptr<Shader> shader) const;

        /// @brief Static material registry used as a factory for basic materials.
        ///
        /// Allows retrieval and reuse of predefined material types.
        static std::unordered_map<std::string, std::shared_ptr<Material>> basicMaterials;

        /// @brief Retrieves or creates a material of the specified type.
        ///
        /// @tparam T Type of the material (must derive from Material).
        /// @param name Name of the material to fetch or create.
        /// @return Shared pointer to the requested material.
        template <typename T>
        static std::shared_ptr<T> getMaterial(
            const std::string& name);

        static std::shared_ptr<Material> FromAsset(const std::shared_ptr<IO::ImageAsset>& imageAsset);

        /// @brief Bind a named texture to this material (generic).
        ///
        /// Example: setTexture("mainTexture", handle) or setTexture("normalMap", handle).
        void setTexture(const std::string& name, TextureHandle handle);

        /// @brief Get a bound texture by name. Returns 0 if not found.
        TextureHandle getTexture(const std::string& name) const;

        /// @brief Remove a texture binding by name.
        void removeTexture(const std::string& name);

        // Backwards-compatible helpers (convenience)
        bool isUsingTexture() const { return useTexture; }
        TextureHandle getMainTexture() const { return mainTexture; }
        Vector2 getTextureScale() const { return mainTextureScale; }

        // Legacy helpers retained for existing call sites: they now propagate to the generic API.
        void useTextureFlag() {
            useTexture = true;
            // Also expose as a uniform property so shaders that rely on "useTexture" get set.
            setProperty<int>("useTexture", 1);
        }
        void setMainTexture(TextureHandle texture) {
            mainTexture = texture;
            setTexture("mainTexture", texture);
        }
        void setTextureScale(const Vector2& scale) {
            mainTextureScale = scale;
            // Ensure shader uniform property gets set by property loop in apply.
            setProperty<Vector2>("mainTextureScale", scale);
        }

    protected:
        /// @brief Persistent material properties (remain until changed).
        PropertyMap properties;
        /// @brief Temporary material properties (cleared frequently).
        PropertyMap tempProperties;

        // Texture bindings stored separately from PropertyMap so textures are always bound
        // by name (and won't be confused with numeric properties).
        std::unordered_map<std::string, TextureHandle> textureBindings;

        // Legacy / convenience members (kept for compatibility)
        bool useTexture = false;
        TextureHandle mainTexture = 0;
        Vector2 mainTextureScale = Vector2(1.0f, 1.0f);
    };

} // namespace LaurelEye::Graphics
