/// @file   Shader.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///    anish.murthy@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   10-09-2025
/// @brief Shader class

#pragma once

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye {
    class Vector3;
    class Matrix4;
} // namespace LaurelEye

namespace LaurelEye::Graphics {

    /// @class Shader
    /// @brief Encapsulates an OpenGL shader program.
    ///
    /// The Shader class manages the creation, linking, and use of GLSL shader
    /// programs. It also provides utility functions for setting uniforms of various
    /// types and managing shader program state.
    ///
    /// This class is typically used by the Material system and the renderer to
    /// bind and update shaders used in draw calls.
    class Shader {
    public:
        /// @brief Deleted copy constructor to prevent copying.
        Shader(const Shader&) = delete;
        /// @brief Deleted copy assignment operator to prevent copying.
        Shader& operator=(const Shader&) = delete;

        /// @brief Destructor. Frees all GPU shader resources.
        ~Shader();

        /// @brief Activates the shader program for rendering.
        void use();
        /// @brief Deactivates the current shader program.
        void unuse();

        // ---------------------------------------------------------------------
        // Uniform setters
        // ---------------------------------------------------------------------

        /* Todo: figure out if we can make this generic using std::variant? */
        /// @brief Sets an unsigned integer uniform variable.
        /// @param name The name of the uniform variable.
        /// @param value The unsigned integer value to set.
        void setUInt(const std::string& name, unsigned int value) const;
        /// @brief Sets an integer uniform variable.
        /// @param name The name of the uniform variable.
        /// @param value The integer value to set.
        void setInt(const std::string& name, int value) const;
        /// @brief Sets a 2-component integer vector uniform.
        /// @param name The name of the uniform variable.
        /// @param v0 The first integer value.
        /// @param v1 The second integer value.
        void set2Int(const std::string& name, int v0, int v1) const;
        /// @brief Sets a floating-point uniform variable.
        /// @param name The name of the uniform variable.
        /// @param value The float value to set.
        void setFloat(const std::string& name, float value) const;
        /// @brief Sets a 2D vector uniform.
        /// @param name The name of the uniform variable.
        /// @param x The X component.
        /// @param y The Y component.
        void setVec2(const std::string& name, float x, float y) const;
        /// @brief Sets a 3D vector uniform.
        /// @param name The name of the uniform variable.
        /// @param value The 3D vector value to set.
        void setVec3(const std::string& name, const Vector3& value) const;
        /// @brief Sets a 4D vector uniform.
        /// @param name The name of the uniform variable.
        /// @param x The X component.
        /// @param y The Y component.
        /// @param z The Z component.
        /// @param w The W component.
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        /// @brief Sets a 4x4 matrix uniform.
        /// @param name The name of the uniform variable.
        /// @param value The 4x4 matrix to upload.
        void setMat4(const std::string& name, const Matrix4& value) const;

        // using Property = std::variant<
        //     unsigned int,
        //     int,
        //     IVector2,
        //     float,
        //     Vector2,
        //     Vector3,
        //     Vector4,
        //     Matrix4>;
        //     // TextureHandle>;
        //
        // using PropertyMap = std::unordered_map<std::string, Property>;

        // TODO: Implement this for QoL
        // void setProperty(const std::string& name, Property property) const;
        // void setProperties(const PropertyMap&) const;

        void bindTexture(
             unsigned int textureUnit,
             const std::string& name,
             TextureHandle textureID,
            TextureType type = TextureType::Texture2D) const;
        void unbindTexture(
            unsigned int textureUnit, TextureType type = TextureType::Texture2D) const;

        //Image Textures
        /*void bindImageTexture(
             unsigned int bindingUnit,
             TextureHandle textureID,
             GLenum access) const;*/

        /// @brief Retrieves the OpenGL program ID.
        /// @return Unsigned integer ID of the shader program.
        unsigned int getId() { return programID; };

        /// @brief Sets the draw mode (e.g., GL_TRIANGLES, GL_LINES).
        /// @param _drawMode OpenGL primitive type.
        void setDrawMode(GLenum _drawMode);
        /// @brief Gets the currently assigned draw mode.
        /// @return The OpenGL primitive type used for drawing.
        GLenum getDrawMode();

        /// @brief Initializes a Uniform Buffer Object (UBO) binding.
        /// @param name The uniform block name in the shader.
        /// @param blockBinding The binding point index.
        void initializeUBO(const std::string& name, unsigned int blockBinding);

    private:
        /// @brief Private constructor used internally by ShaderManager.
        /// @param shaderFilePaths Paths to the shader files to load.
        Shader(const std::string& shaderFilePaths);
        /// @brief Map of shader types to their respective compiled IDs.
        std::unordered_map<GLenum, std::vector<GLuint>> shaderIDs;
        /// @brief Cache for uniform locations to reduce glGetUniformLocation calls.
        /// @note Mutable to allow caching in const uniform setter functions.
        mutable std::unordered_map<std::string, GLint> uniformLocationCache; // Todo: Why is this mutable?
        /// @brief The OpenGL program ID for this shader.
        GLuint programID;
        /// @brief The draw mode for this shader (e.g., GL_TRIANGLES, GL_LINES).
        GLenum drawMode = 0;

        // ---------------------------------------------------------------------
        // Internal helpers
        // ---------------------------------------------------------------------

        /// @brief Get shader type.
        ///
        /// @param path Shader file path.
        /// @return OpenGL shader type enum.
        GLenum readShaderType(std::string path);
        /// @brief Get a string name for the shader type. Used for debug
        /// printing.
        ///
        /// @param type OpenGL shader type enum.
        std::string shaderTypeToString(GLenum type);
        /// @brief Get contents of a shader file as string.
        ///
        /// @param path Shader file path.
        std::string readShaderFile(std::string path);
        GLuint loadShader(GLenum type, const GLchar* source);
        void deleteShader(GLuint id);
        void linkShaders();
        void cacheUniforms();
        GLint getUniformLocation(const std::string& name) const;

        std::string toLowerCase(const std::string& str);

        /// @brief ShaderManager is a friend class, allowing it to create and manage shaders.
        friend class ShaderManager;
    };
} // namespace LaurelEye::Graphics
