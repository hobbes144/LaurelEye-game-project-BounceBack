/// @file   Shader.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Shader for OpenGL

#include "LaurelEyeEngine/graphics/resources/Shader.h"

#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/logging/EngineLog.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

#include <fstream>
#include <sstream>
#include <string>

namespace LaurelEye::Graphics {
    void Shader::setDrawMode(GLenum _drawMode) {
        drawMode = _drawMode;
    }

    GLenum Shader::getDrawMode() {
        return drawMode;
    }

    /*!****************************************************************************
     * \brief Shader constructor
     *
     * Constructor will create the relevant shaders and link them to the Shader
     * program.
     *
     * \param shaderFilePaths String with shader file paths separated by newline.
     *****************************************************************************/
    Shader::Shader(const std::string& name, const std::string& shaderFilePaths) {
        std::istringstream pathsStream(shaderFilePaths);
        std::string shaderFilePath;

        while ( std::getline(pathsStream, shaderFilePath) ) {
            GLenum type = readShaderType(shaderFilePath);
            std::string source = readShaderFile(shaderFilePath);
            shaderIDs[type].push_back(loadShader(type, source.c_str()));
        }

        linkShaders();
        cacheUniforms();

        glObjectLabel(GL_PROGRAM, programID, name.size(), name.c_str());

        for ( const auto& [key, ids] : shaderIDs ) {
            for ( const auto& id : ids ) {
                deleteShader(id);
            }
        }
    }

    /*!****************************************************************************
     * \brief Shader destructor
     *
     *****************************************************************************/
    Shader::~Shader() {
        glDeleteProgram(programID);
    }

    /*!****************************************************************************
     * \brief Helper function to convert a string to lowercase
     *
     * \param str String to be covnerted to lowercase.
     * \return \b std::string The lowercase string.
     *****************************************************************************/
    std::string Shader::toLowerCase(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return lowerStr;
    }

    /*!****************************************************************************
     * \brief Get shader type from file name
     *
     * \param path Shader file name.
     * \return \b GLenum Shader type.
     *****************************************************************************/
    GLenum Shader::readShaderType(std::string path) {
        std::string lowercasePath = toLowerCase(path);

        if ( lowercasePath.find(".vert") != std::string::npos )
            return GL_VERTEX_SHADER;
        if ( lowercasePath.find(".frag") != std::string::npos )
            return GL_FRAGMENT_SHADER;
        if ( lowercasePath.find(".geom") != std::string::npos )
            return GL_GEOMETRY_SHADER;
        if ( lowercasePath.find(".comp") != std::string::npos )
            return GL_COMPUTE_SHADER;
        else
            return GL_VERTEX_SHADER;
    }

    std::string Shader::shaderTypeToString(GLenum type) {
        if ( type == GL_VERTEX_SHADER )
            return "VERTEX";
        if ( type == GL_FRAGMENT_SHADER )
            return "FRAGMENT";
        if ( type == GL_GEOMETRY_SHADER )
            return "GEOMETRY";
        if ( type == GL_COMPUTE_SHADER )
            return "COMPUTE";
        else
            return "VERTEX";
    }

    /*!****************************************************************************
     * \brief Read shader data from a file
     *
     * \param path File path of the shader.
     * \return \b std::string Shader file contents.
     *****************************************************************************/
    std::string Shader::readShaderFile(std::string path) {
        std::ifstream shaderFile(path);
        LE_ASSERT("graphics", shaderFile.is_open(), "Shader " << name << ": Unable to open shader file at path: " << path);

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();

        shaderFile.close();

        return buffer.str();
    }

    /*!****************************************************************************
     * \brief Create glShader object
     *
     * \param type Type of shader to be created.
     * \param source Source code of the shader.
     * \return \b GLuint Shader ID of the new Shader.
     *****************************************************************************/
    GLuint Shader::loadShader(GLenum type, const GLchar* source) {
        int success;
        char infoLog[512];
        GLuint shaderId;

        shaderId = glCreateShader(type);

        glShaderSource(shaderId, 1, &source, NULL);
        glCompileShader(shaderId);

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if ( !success ) {
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            LE_ASSERT("graphics", success, "Shader " << name << ": " << shaderTypeToString(type) << " shader compilation failed, error: " << infoLog);
        }

        return shaderId;
    }

    /*!****************************************************************************
     * \brief Delete glShader object
     *
     * \param id ID of the shader to be deleted.
     *****************************************************************************/
    void Shader::deleteShader(GLuint id) {
        glDeleteShader(id);
    }

    /*!****************************************************************************
     * \brief Link glShaders and create the Shader Program
     *
     *****************************************************************************/
    void Shader::linkShaders() {
        int success;
        char infoLog[512];

        programID = glCreateProgram();

        for ( const auto& [type, ids] : shaderIDs ) {
            for ( const auto& id : ids ) {
                glAttachShader(programID, id);
            }
        }

        glLinkProgram(programID);

        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if ( !success ) {
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            LE_ASSERT("graphics", success, "Shader " << name << ": Shader program compilation failed, error: " << infoLog);
        }
    }

    void Shader::cacheUniforms() {
        // Getting number of uniforms in the Shader Program
        // Note: This does not cache Uniform Blocks.
        // We will handle that later as needed.
        GLint uniformCount;
        glGetProgramInterfaceiv(
            programID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniformCount);

        const GLenum properties[4] = {
            GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};

        for ( int uniformNumber = 0; uniformNumber < uniformCount; ++uniformNumber ) {
            GLint values[4];
            glGetProgramResourceiv(programID, GL_UNIFORM, uniformNumber, 4,
                                   properties, 4, NULL, values);

            if ( values[0] != -1 )
                continue;

            std::vector<char> nameData(values[2]);
            glGetProgramResourceName(programID, GL_UNIFORM, uniformNumber,
                                     static_cast<GLsizei>(nameData.size()), NULL, &nameData[0]);
            std::string name(nameData.begin(), nameData.end() - 1);
            uniformLocationCache[name] = values[3];

            glUseProgram(programID);
            switch ( values[1] ) {
            case GL_SAMPLER_CUBE:
                glUniform1i(values[3], InvalidSamplerCubeBinding);
                break;
            case GL_SAMPLER_2D:
                glUniform1i(values[3], InvalidSampler2DBinding);
                break;
            case GL_SAMPLER_3D:
                glUniform1i(values[3], InvalidSampler3DBinding);
                break;
            }
            glUseProgram(0);
        }
    }

    /*!****************************************************************************
     * \brief Function to use the Shader Program
     *
     * This should be used every time a render pass needs to use this specific
     * shader.
     *
     *****************************************************************************/
    void Shader::use() {
        glUseProgram(programID);
    }

    /*!****************************************************************************
     * \brief Function to unuse the Shader Program
     *
     * This should be used every time a render pass is completed.
     *
     *****************************************************************************/
    void Shader::unuse() {
        glUseProgram(0);
    }

    /*!****************************************************************************
     * \brief Cache and fetch for Uniform locations
     *
     * \param name Name of the uniform.
     * \return \b GLint Uniform ID.
     *****************************************************************************/
    GLint Shader::getUniformLocation(const std::string& name) const {
        auto it = uniformLocationCache.find(name);

        if ( it != uniformLocationCache.end() ) {
            return it->second;
        }
        GLint location = glGetUniformLocation(programID, name.c_str());
        uniformLocationCache[name] = location;
        LE_DEBUG_WARN_IF("graphics", location >= 0, "Shader " << name << ": Uniform " << name << " requested but doesn't exist.");

        return location;
    }

    void Shader::initializeUBO(const std::string& name, unsigned int blockBinding) {
        unsigned int uniformBlockIndex =
            glGetUniformBlockIndex(programID, name.c_str());
        glUniformBlockBinding(programID, uniformBlockIndex, blockBinding);
        LE_DEBUG_INFO("graphics", "Shader " << name << ": UBO initialized at "<< uniformBlockIndex << ".");
    }

    /*!****************************************************************************
     * \brief Set UInt Uniform
     *
     * \param name Name of the uniform.
     * \param value Value of the uniform
     *****************************************************************************/
    void Shader::setUInt(const std::string& name, unsigned int value) const {
        glUniform1ui(getUniformLocation(name), value);
    }

    /*!****************************************************************************
     * \brief Set Int Uniform
     *
     * \param name Name of the uniform.
     * \param value Value of the uniform
     *****************************************************************************/
    void Shader::setInt(const std::string& name, int value) const {
        glUniform1i(getUniformLocation(name), value);
    }

    /*!****************************************************************************
     * \brief Set 2Int Uniform
     *
     * \param name Name of the uniform.
     * \param v0 Value of the uniform
     * \param v1 Value of the uniform
     *****************************************************************************/
    void Shader::set2Int(const std::string& name, int v0, int v1) const {
        glUniform2i(getUniformLocation(name), v0, v1);
    }

    /*!****************************************************************************
     * \brief Set Float Uniform
     *
     * \param name Name of the uniform.
     * \param value Value of the uniform
     *****************************************************************************/
    void Shader::setFloat(const std::string& name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }

    /*!****************************************************************************
     * \brief Set 2Float Uniform
     *
     * \param name Name of the uniform.
     * \param x Value of the uniform
     * \param y Value of the uniform
     *****************************************************************************/
    void Shader::setVec2(const std::string& name, float x, float y) const {
        glUniform2f(getUniformLocation(name), x, y);
    }

    /*!****************************************************************************
     * \brief Set Vector3 Uniform
     *
     * \param name Name of the uniform.
     * \param value Value of the uniform
     *****************************************************************************/
    void Shader::setVec3(const std::string& name, const Vector3& value) const {
        glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
    }

    /*!****************************************************************************
     * \brief Set Vector4 Uniform
     *
     * \param name Name of the uniform.
     * \param x Value of the uniform
     * \param y Value of the uniform
     * \param z Value of the uniform
     * \param w Value of the uniform
     *****************************************************************************/
    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    /*!****************************************************************************
     * \brief Set Matrix4 Uniform
     *
     * \param name Name of the uniform.
     * \param value Value of the uniform
     *****************************************************************************/
    void Shader::setMat4(const std::string& name, const Matrix4& value) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value.getData());
    }

    void Shader::bindTexture(
        unsigned int textureUnit,
        const std::string& name,
        TextureHandle textureID, TextureType type) const {

#if !defined(NDEBUG)
        // Validate texture unit against device limits to avoid GL_INVALID_ENUM.
        GLint maxUnits = 0;
        // LE_ASSERT("Graphics", textureUnit < 10, "Texture " << name << " bound here");
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        if ( static_cast<GLint>(textureID) >= maxUnits ) {
            LE_ASSERT("graphics", static_cast<GLint>(textureID) >= maxUnits, "Shader " << name << ": Invalid texture unit being bound!");
            return;
        }
#endif

        glBindTextureUnit(textureUnit, textureID);
        setInt(name, static_cast<int>(textureUnit));
    }

    void Shader::bindTexture(
        unsigned int textureUnit,
        TextureHandle textureID) const {

#if !defined(NDEBUG)
        // Validate texture unit against device limits to avoid GL_INVALID_ENUM.
        GLint maxUnits = 0;
        // LE_ASSERT("Graphics", textureUnit < 10, "Texture " << name << " bound here");
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
        if ( static_cast<GLint>(textureID) >= maxUnits ) {
            LE_ASSERT("graphics", static_cast<GLint>(textureID) >= maxUnits, "Shader " << name << ": Invalid texture unit being bound!");
            return;
        }
#endif

        glBindTextureUnit(textureUnit, textureID);
    }

    void Shader::unbindTexture(
        unsigned int textureUnit,
        TextureType type) const {
        glBindTextureUnit(textureUnit, 0);
    }

    // void Shader::bindImageTexture(
    //   unsigned int bindingUnit,
    //   TextureHandle texHandle,
    //   GLenum access) const
    //{

    //  //Find a way to get the format from the texHandle for the last argument
    //  glBindImageTexture(
    //    bindingUnit, texHandle, 0, GL_FALSE, 0, access, );
    //}

} // namespace LaurelEye::Graphics
