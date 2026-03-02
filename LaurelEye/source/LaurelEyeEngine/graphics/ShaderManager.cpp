/// @file   ShaderManager.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of ShaderManager

#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"

namespace LaurelEye::Graphics {

    std::shared_ptr<Shader> ShaderManager::loadFile(const std::string& name, const std::string& filePath) {
        if ( loadedFiles.contains(filePath) )
            return loadedFiles[filePath];

        loadedFiles[filePath] = std::shared_ptr<Shader>(new Shader(name, filePath));
        return loadedFiles[filePath];
    }

    void ShaderManager::unloadShader(std::shared_ptr<Shader> shader) {
        std::erase_if(loadedFiles, [&](auto& kv) { return kv.second == shader; });
    }

    void ShaderManager::unloadFile(const std::string& filePath) {
        loadedFiles.erase(filePath);
    }

    void ShaderManager::unloadShaders() {
        // TODO: Fix this, because using shared pointers doesn't guarantee the shaders are destroyed until all refs are deleted.
        loadedFiles.clear();
    }

} // namespace LaurelEye::Graphics
