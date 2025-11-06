/// @file   GeometryBuffer.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  OpenGL geometry buffer abstraction managing VAO, VBO, and EBO resources.
///
/// The `GeometryBuffer` class encapsulates OpenGL buffer objects used to store
/// vertex attribute data (positions, normals, UVs, etc.) and optional index data.
/// It provides creation, updating, and binding utilities while maintaining
/// a high-level abstraction over OpenGL buffer management.
///
/// This class is typically used by mesh and rendering systems to efficiently
/// manage vertex data in GPU memory.

#pragma once

// TODO: Move GL operations to the Device
#include <glad/glad.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace LaurelEye::Graphics {

    /// @class GeometryBuffer
    /// @brief Encapsulates OpenGL vertex and index buffers for a mesh.
    ///
    /// The `GeometryBuffer` manages a VAO (Vertex Array Object),
    /// a VBO (Vertex Buffer Object), and optionally an EBO (Element Buffer Object)
    /// for indexed drawing. It supports both interleaved and non-interleaved
    /// attribute layouts, and provides mechanisms for dynamic updates of vertex data.
    class GeometryBuffer {
    public:
        /**
          \brief Attribute types supported by the code

          These are also the positions of each attribute in the GPU.
        */

        /// @enum AttributeType
        /// @brief Attribute types supported by the GeometryBuffer.
        /// These correspond to specific vertex attribute locations in the shader program.
        enum class AttributeType {
            Position, ///< Vertex position attribute.       0
            Normal,   ///< Vertex normal attribute.         1
            TexCoord, ///< Texture coordinate attribute.    2
            Color,    ///< Vertex color attribute.          3
            Tangent,  ///< Tangent vector attribute.        4
            Bitangent, ///< Bitangent vector attribute.     5
            ParticleSize, ///< Particle Size                6
            ParticleRotation ///< Particle Rotation         7
        };

        /// @enum AttributeInfo
        /// @brief Attribute types supported by the GeometryBuffer.
        /// These correspond to specific vertex attribute locations in the shader program.
        struct AttributeInfo {
            /// @brief Flat vector containing attribute data (size = dimensions × vertex count).
            std::vector<float> data;
            /// @brief Number of components per attribute (e.g., 3 for vec3).
            GLint elementSize;
            /// @brief OpenGL type (e.g., GL_FLOAT).
            GLenum type;
            /// @brief Whether values should be normalized.
            GLboolean normalized;
            /// @brief Byte offset between consecutive vertex attributes.
            GLsizei stride;
        };

        /// @brief Modifiable version of GeometryBuffer::Attributes.
        using ModifiableAttributes = std::unordered_map<
            AttributeType,
            AttributeInfo>;

        
        /// @brief Attribute input data structure
        ///
        /// Attributes stores:
        /// - GeometryBuffer::AttributeType (position or color, etc)
        /// - GeometryBuffer::AttributeInfo:
        ///   - std::vector<float>  data: 1D vector of size (Dimension * numpoints)
        ///   - GLint               elementSize: Number of dimensions
        ///   - GLenum              type: GL type
        ///   - GLboolean           normalized: If should normalize 
        using Attributes = const std::unordered_map<
            AttributeType,
            AttributeInfo>;

        /// @brief Destructor. Cleans up GPU resources.
        ~GeometryBuffer();

        /// @brief Deleted copy constructor to prevent copying.
        GeometryBuffer(const GeometryBuffer&) = delete;
        /// @brief Deleted copy assignment operator to prevent copying.
        GeometryBuffer& operator=(const GeometryBuffer&) = delete;

        /// @brief Move constructor.
        GeometryBuffer(GeometryBuffer&&) noexcept;
        /// @brief Move assignment operator.
        GeometryBuffer& operator=(GeometryBuffer&&) noexcept;

        /// @brief Factory method to create a GeometryBuffer with vertex attributes only.
        ///
        /// @param attributeData Vertex attribute data and layout information.
        /// @param name Human-readable name for debugging.
        /// @param bufferIsInterleaved Whether attributes are interleaved in memory.
        /// @return Shared pointer to a new GeometryBuffer.
        static std::shared_ptr<GeometryBuffer> create(
            Attributes& attributeData,
            const std::string& name,
            bool bufferIsInterleaved = true);
        /// @brief Factory method to create a GeometryBuffer with vertex and index data.
        ///
        /// @param attributeData Vertex attribute data and layout information.
        /// @param indices Index data for element drawing.
        /// @param name Human-readable name for debugging.
        /// @param bufferIsInterleaved Whether attributes are interleaved in memory.
        /// @return Shared pointer to a new GeometryBuffer.
        static std::shared_ptr<GeometryBuffer> create(
            Attributes& attributeData,
            const std::vector<unsigned int>& indices,
            const std::string& name,
            bool bufferIsInterleaved = true);

        void destroy();

        /// @brief Updates an existing vertex attribute's data.
        ///
        /// @param type Attribute type to update.
        /// @param data New vertex data.
        void updateVertexAttribute(
            const AttributeType& type,
            const std::vector<float>& data);
        /// @brief Updates the index buffer data.
        ///
        /// @param indices New index data.
        void updateIndices(const std::vector<unsigned int>& indices);

        /// @brief Checks whether the buffer contains a given attribute.
        ///
        /// @param type Attribute type to query.
        /// @return True if the attribute exists; false otherwise.
        bool hasAttribute(AttributeType type);

        /// @brief Binds the GeometryBuffer for rendering.
        void bind() const;
        /// @brief Unbinds the GeometryBuffer.
        void unbind() const;

        /// @brief Returns the number of vertices stored in the buffer.
        ///
        /// @return GLsizei Vertex count.
        GLsizei getVertexCount() const { return vertexCount; }

        /// @brief Returns the number of indices stored in the buffer.
        ///
        /// @return GLsizei Index count.
        GLsizei getIndexCount() const { return indexCount; }

        /// @brief Returns a copy of the index data stored on the CPU side.
        ///
        /// @return std::vector<unsigned int> containing index data.
        std::vector<unsigned int> getIndexData() const { return indexData; }

        /// @brief Returns the OpenGL VAO ID.
        ///
        /// @return GLuint VAO handle.
        GLuint getVAO() const { return vao; }

        /// @brief Returns the OpenGL VBO ID.
        ///
        /// @return GLuint VBO handle.
        GLuint getVBO() const { return vbo; }

        /// @brief Returns the OpenGL EBO ID.
        ///
        /// @return GLuint EBO handle.
        GLuint getEBO() const { return ebo; }

    private:
        /// @brief Vertex Array Object ID.
        GLuint vao;
        /// @brief Vertex Buffer Object ID.
        GLuint vbo;
        /// @brief Element Buffer Object ID.
        GLuint ebo;

        /// @brief Whether attribute data is interleaved.
        bool isInterleaved;

        /// @brief Number of Vertices 
        GLsizei vertexCount;
        /// @brief Number of Indices 
        GLsizei indexCount;

        /// @brief Currently bound VAO for static tracking.
        static GLuint boundVAO;

        /// @brief Byte offset for each attribute in the vertex buffer.
        std::unordered_map<AttributeType, GLsizeiptr> attributeOffsets;

        /* Todo: Convert to interleaved and use the below */
        // std::unordered_map<AttributeType, GLsizei> attributeStrides;

        /// @brief Buffer name (for debugging and profiling).
        std::string name;

        /// @brief CPU-side copy of vertex attribute data.
        ModifiableAttributes bufferAttributeData;

        /// @brief CPU-side copy of index data.
        std::vector<unsigned int> indexData;

        /// @brief Private constructor used by factory functions.
        GeometryBuffer(std::string name) : vao(0), vbo(0), ebo(0), vertexCount(0),
                                           indexCount(0), name(name), isInterleaved(true) {};

        /// @brief Initializes vertex buffer data.
        void initializeVertexBuffers(
            Attributes& attributeData);
        /// @brief Initializes element/index buffer data.
        void initializeElementBuffers(const std::vector<unsigned int>& indices);

        /// @brief Initializes buffer data for non-indexed geometry.
        void initializeBuffers(
            Attributes& attributeData);
        /// @brief Initializes buffer data for indexed geometry.
        void initializeBuffers(
            Attributes& attributeData,
            const std::vector<unsigned int>& indices);

        GLenum getGlType(AttributeType);

        /// @brief Cleans up all GPU resources associated with this buffer.
        void cleanupBuffers();
    };

} // namespace LaurelEye::Graphics
