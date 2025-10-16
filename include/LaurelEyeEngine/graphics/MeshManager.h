/// @file   MeshManager.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-15-2025
/// @brief Mesh Manager class that manages created meshes for the GPU.
///
/// Note: this is currently a stub. Actual logic is in the Mesh class and will
/// be moved here in the future.
#pragma once

#include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"

namespace LaurelEye::Graphics {

    class MeshManager {
    public:
        MeshManager(GraphicsBackend type = GraphicsBackend::OpenGL);

        void initialize();            // Dummy call, but may want to auto create main window?
        void update(float deltaTime); // Dummy call
        void shutdown();              // Dummy call

        // Mesh* createMesh(const MeshDescription& wDesc);
        Mesh* createMesh(Mesh::Type);
        Mesh* getMesh(int id);

    private:
        GraphicsBackend systemType;
        std::vector<std::unique_ptr<Mesh>> managedMeshes;

    };

}
