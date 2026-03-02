/// @file   ShaderManager.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Centralized manager responsible for loading, caching, and unloading shader programs.
///
/// The ShaderManager implements a singleton pattern that ensures shaders are loaded once
/// and shared across the engine. It maintains a cache of compiled shader programs keyed by
/// their file paths, allowing efficient reuse and preventing redundant GPU allocations.

#pragma once

#include <memory>
#include <unordered_map>

namespace LaurelEye::Graphics {

    class Shader;

    /// @class ShaderManager
    /// @brief Singleton class that handles the lifecycle and caching of shader programs.
    ///
    /// The `ShaderManager` ensures that each shader file is only loaded once and shared among
    /// all systems or components that request it. It provides functionality to load shaders
    /// from disk, retrieve them as shared pointers, and unload them when no longer needed.
    ///
    /// Example usage:
    /// @code
    /// auto& shaderManager = ShaderManager::getInstance();
    /// std::shared_ptr<Shader> myShader = shaderManager.loadFile("shaders/basic.glsl");
    /// @endcode
    class ShaderManager {
    public:
        /// @brief Retrieves the global instance of the ShaderManager.
        /// @return Reference to the singleton ShaderManager instance.
        static ShaderManager& getInstance() {
            static ShaderManager instance;
            return instance;
        }

        /// @brief Loads a shader from a file path or returns a cached instance if already loaded.
        ///
        /// If the shader specified by the given file path has already been loaded,
        /// the existing shared instance is returned instead of recompiling it.
        ///
        /// @param filePath Path to the shader source file.
        /// @return Shared pointer to the loaded shader object.
        std::shared_ptr<Shader> loadFile(const std::string& name, const std::string& filePath);
        /// @brief Unloads a specific shader instance.
        ///
        /// This method removes a shader from the cache if it matches the specified instance.
        /// The shader will only be destroyed when all shared references are released.
        ///
        /// @param shader Shared pointer to the shader to unload.
        void unloadShader(std::shared_ptr<Shader> shader);
        /// @brief Unloads a shader from the cache based on its file path.
        /// @param filePath Path of the shader to unload.
        void unloadFile(const std::string& filePath);
        /// @brief Unloads all shaders currently stored in the cache.
        ///
        /// This should typically be called during renderer shutdown or when reloading the graph
        void unloadShaders();

    private:
        /// @brief Private constructor to enforce singleton pattern.
        ShaderManager() = default;
        /// @brief Map of loaded shader file paths to their shared shader instances.
        std::unordered_map<std::string, std::shared_ptr<Shader>> loadedFiles;
    };

} // namespace LaurelEye::Graphics
