/// @file   Mesh.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Mesh class responsible for storing and managing vertex, index, and geometry buffer data.
///
/// The Mesh class encapsulates the data required to represent and render 3D (and 2D) geometry.
/// It interfaces with the GeometryBuffer to manage vertex attributes, index buffers,
/// and GPU upload operations. Mesh instances can be cloned, loaded from files, or
/// generated procedurally.

#pragma once

#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/math/Transform.h"
#include "LaurelEyeEngine/io/Assets.h"

// TODO: Move GL operations to the Device
#include <glad/glad.h>

#include "assimp/mesh.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

constexpr size_t max_vertices_per_bone = 4;

namespace LaurelEye::Graphics {
    /// @class Mesh
    /// @brief Represents geometric data for renderable objects in the engine.
    ///
    /// A Mesh defines vertex attribute data (positions, normals, UVs, etc.)
    /// and optionally an index buffer. It manages a `GeometryBuffer` that handles
    /// the actual GPU-side storage of vertex data.
    class Mesh {
    public:
        /// @class SkinData
        /// @brief Mesh vertice to bone relationships.
        ///
        /// This stores the vertex modification weight from the bone
        /// transforms. This is particularly important to ensure things like
        /// arm joints behave correctly.
        ///
        struct SkinData {
            int ids[max_vertices_per_bone] = {0};
            float w[max_vertices_per_bone] = {0.0f};

            void AddSkinData(int _id, float _w) {
                int arr_size = sizeof(ids) / sizeof(ids[0]);
                for ( int i = 0; i < arr_size; i++ ) {
                    if ( w[i] == 0.0 ) {
                        ids[i] = _id;
                        w[i] = _w;
                        return;
                    }
                }

                assert(0);
            }
        };

        /// @brief Attribute storage type mapping GeometryBuffer attributes to their data and element size.
        ///
        /// Each entry maps an `AttributeType` (e.g., Position, Normal) to a pair containing:
        /// - A vector of floats with vertex data.
        /// - The number of components per vertex (e.g., 3 for position).
        using Attributes =
            std::unordered_map<
                GeometryBuffer::AttributeType,
                std::pair<
                    std::vector<float>,
                    GLint>>;

        /// @brief Common debug and primitive mesh types.
        enum Type {
            Square,
            Cube,
            Sphere,
            None
        };

        /// @brief Constructs an empty Mesh with the given name.
        /// @param name Unique mesh identifier.
        Mesh(const std::string& name);
        /// @brief Constructs a Mesh using vertex attribute data.
        /// @param name Mesh name.
        /// @param attrData Vertex attribute data map.
        /// @param interleavedStride Optional stride if data is interleaved.
        Mesh(
            const std::string& name,
            Attributes attrData,
            const GLsizei interleavedStride = 0);
        /// @brief Constructs a Mesh using vertex attributes and indices.
        /// @param name Mesh name.
        /// @param attrData Vertex attribute data map.
        /// @param indices Index buffer data.
        /// @param interleavedStride Optional stride if data is interleaved.
        Mesh(
            const std::string& name,
            Attributes attrData,
            const std::vector<unsigned int>& indices,
            const GLsizei interleavedStride = 0);
        /// @brief Virtual destructor for cleanup.
        virtual ~Mesh() = default;

        /// @brief Sets the GeometryBuffer used by this Mesh.
        /// @param geometryBuffer Shared pointer to a GeometryBuffer.
        void setVertexData(const std::shared_ptr<GeometryBuffer>& geometryBuffer);

        /// @brief Sets or updates a specific vertex attribute.
        /// @param type Attribute type (Position, Normal, TexCoord, etc.).
        /// @param data Flat array of vertex data.
        /// @param componentsPerVertex Number of components per vertex.
        /// @param interleavedStride Optional interleaved stride value.
        void setAttributeData(
            GeometryBuffer::AttributeType& type,
            const std::vector<float>& data,
            int componentsPerVertex,
            const GLsizei interleavedStride);
        /// @brief Updates the Mesh’s index data.
        /// @param newIndices Vector of indices.
        void setIndices(const std::vector<unsigned int>& newIndices);

        /// @brief Retrieves the GeometryBuffer associated with this Mesh.
        /// @return Shared pointer to the GeometryBuffer.
        std::shared_ptr<GeometryBuffer> getGeometryBuffer() const {
            return geometryBuffer;
        }

        /// @brief Creates a deep copy of this Mesh.
        /// @return Unique pointer to the cloned Mesh.
        std::unique_ptr<Mesh> clone() const;

        /// @brief Returns the number of vertices in this Mesh.
        /// @return number of vertices in this Mesh
        size_t getVertexCount() const;
        /// @brief Returns the number of indices in this Mesh.
        /// @return number of indices in this Mesh
        size_t getIndexCount() const;

        /// @brief Checks if the Mesh contains a specific attribute.
        /// @param attr The attribute type to check for.
        /// @return True if the attribute exists, false otherwise.
        bool hasAttribute(GeometryBuffer::AttributeType attr) const;
        /// @brief Retrieves the Mesh name.
        /// @return Mesh name string.
        const std::string& getName() const;

        /// @brief Draws the Mesh using the provided draw mode (default: GL_TRIANGLES).
        /// @param mode OpenGL draw mode (e.g., GL_TRIANGLES, GL_LINES).
        void draw(GLenum mode = GL_TRIANGLES);
        /// @brief Draws triangles from the EBO without rebinding buffers.
        void drawEBOTrisWithoutBind();

        /* ---------------------------------------------------------------------- */
        /*                           Mesh Factory Methods                          */
        /* ---------------------------------------------------------------------- */

        /// @brief Utility for constructing a quad from vertex indices.
        static void pushquad(std::vector<unsigned int>& Tri, int i, int j, int k, int l);
        /// @brief Loads a mesh from an external file (e.g., OBJ).
        /// @param filename File path.
        /// @return Shared pointer to the loaded Mesh.
        static std::shared_ptr<Mesh> loadMesh(const std::string& filename);
        /// @brief Creates a face mesh using a transform.
        /// @param tr Transform applied to the face.
        /// @return Pair containing Attributes and indices for the face.
        static std::pair<Attributes, std::vector<unsigned int>> createFace(const Transform& tr);

        /// @brief Creates a UV sphere mesh.
        /// @param name Mesh name.
        /// @param n Segments used for generation.
        /// @return Shared pointer to the sphere Mesh.
        static std::shared_ptr<Mesh> createSphereMesh(const std::string& name, const int n = 32);
        /// @brief Creates a Mesh from a predefined Type (Square, Cube, Sphere).
        static std::shared_ptr<Mesh> createMesh(const std::string& name, Type type);
        /// @brief Creates a Mesh from an imported Mesh Asset
        static std::shared_ptr<Mesh> createMeshFromAsset(const std::shared_ptr<IO::MeshAsset>& meshAsset);
        /// @brief Creates a simple 2D square mesh.
        static std::shared_ptr<Mesh> createSquareMesh(const std::string& name, float scale = 1.0f);
        /// @brief Creates a cube mesh.
        static std::shared_ptr<Mesh> createCubeMesh(const std::string& name);
        /// @brief Retrieves a cached shape Mesh of a given type.
        static std::shared_ptr<Mesh> getShapeMesh(Type type);
        /// @brief Creates a teapot mesh for testing or visualization.
        static std::shared_ptr<Mesh> createTeapotMesh(const std::string& name, const int n = 12);

        /// @brief Clears all loaded Mesh buffers.
        static void clearBuffers();

    private:
        /// @brief GPU-backed GeometryBuffer containing vertex data.
        std::shared_ptr<GeometryBuffer> geometryBuffer;
        /// @brief Index buffer data for drawing with EBOs.
        std::vector<unsigned int> indices;

        /// @brief Skin data.
        std::vector<SkinData> skin;

        /// @brief Combines multiple attribute maps into a single Attributes object.
        static Attributes combineAttributes(const std::vector<Attributes>& attributesList);
        /// @brief Merges index lists from multiple meshes with vertex offset correction.
        static void combineIndices(unsigned int previousVertices, std::vector<unsigned int>& destIndices, std::vector<unsigned int> newIndices);
        // static void processMesh(aiMesh* mesh, Attributes& newMeshData, std::vector<unsigned int>& indices);
        /// @brief Cache of loaded meshes for reuse and deduplication.
        static std::unordered_map<std::string, std::shared_ptr<Mesh>> loadedMeshes;

        static void processMesh(aiMesh* mesh, Attributes& newMeshData, std::vector<unsigned int>& indices);

        /// @brief Cached built-in primitive meshes (Square, Cube, Sphere).
        static std::unordered_map<Type, std::shared_ptr<Mesh>> shapeMeshes;

        /// @brief Converts attribute data into GeometryBuffer-compatible format.
        void prepareAttributeData(
            GeometryBuffer::ModifiableAttributes& triangleBufferData,
            const Mesh::Attributes& attrData,
            const GLsizei stride = 0);

        /// @brief Mesh name identifier.
        std::string name;
    };
} // namespace LaurelEye::Graphics
