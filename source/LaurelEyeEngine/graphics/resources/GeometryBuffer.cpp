/*!****************************************************************************
 * \file   GeometryBuffer.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
 *   (zaldivaralvarez@gmail.com) (Advising and Base Code)
 * \par    **Course**
 *    CS529
 * \date   10-14-2024
 *
 * Copyright � 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"

namespace LaurelEye::Graphics {

    GLuint GeometryBuffer::boundVAO = 0;

    /*!****************************************************************************
     * \brief GeometryBuffer Object Factory
     *
     * \param attributeData Attribute type information used to construct the VAO.
     * \param name Name of the Buffer.
     * \param bufferIsInterleaved Set to true if the data is in a single block with
     *  interleaved data. If false, each attribute is considered to have its own
     *  data that needs to be stored in buffer as blocks.
     *  Defaults to true.
     * \return \b std::shared_ptr<GeometryBuffer> Initialized GeometryBuffer object
     *****************************************************************************/
    std::shared_ptr<GeometryBuffer> GeometryBuffer::create(
        Attributes& attributeData,
        const std::string& name,
        bool bufferIsInterleaved) {

        auto buffer = std::shared_ptr<GeometryBuffer>(new GeometryBuffer(name));
        /*
         * The above calls the constructor from external object make_shared which
         * fails since it is private.
         * This specifically calls the private constructor here and then sends the
         * object to the shared_ptr instead.
         */
        buffer->isInterleaved = bufferIsInterleaved;
        buffer->initializeBuffers(attributeData);
        return buffer;
    }

    /*!****************************************************************************
     * \brief GeometryBuffer Object Factory
     *
     * \param attribute Data Attribute type information used to construct the VAO.
     * \param indices Indices to be used to construct the EBO.
     * \param name Name of the Buffer.
     * \param bufferIsInterleaved Set to true if the data is in a single block with
     *  interleaved data. If false, each attribute is considered to have its own
     *  data that needs to be stored in buffer as blocks.
     *  Defaults to true.
     * \return \b std::shared_ptr<GeometryBuffer> Initialized GeometryBuffer object
     *****************************************************************************/
    std::shared_ptr<GeometryBuffer> GeometryBuffer::create(
        Attributes& attributeData,
        const std::vector<unsigned int>& indices,
        const std::string& name,
        bool bufferIsInterleaved) {
        // factory implementation
        auto buffer = std::shared_ptr<GeometryBuffer>(new GeometryBuffer(name));
        /*
         * std::make_shared<GeometryBuffer>(name); <-this won't work
         * because even though std::make_shared is not restricted by the
         * fact that create is static, it internally tries to access the
         * constructor, and since the constructor is private, it can't.
         */
        buffer->isInterleaved = bufferIsInterleaved;
        buffer->initializeBuffers(
            attributeData,
            indices);
        return buffer;
    }

    /*!****************************************************************************
     * \brief Destructor which runs cleanup
     *
     *****************************************************************************/
    GeometryBuffer::~GeometryBuffer() {
        cleanupBuffers();
    }

    /*!****************************************************************************
     * \brief Move Constructor
     *
     * \note We do not cleanup other's buffers since this object will now control
     * them. No new buffers are created in this object.
     *
     * \param other GeometryBuffer object to be moved from.
     *****************************************************************************/
    GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept : vao(other.vao), vbo(other.vbo), ebo(other.ebo),
                                                                      vertexCount(other.vertexCount), indexCount(other.indexCount),
                                                                      indexData(other.indexData), bufferAttributeData(other.bufferAttributeData) {
        other.vao = other.vbo = other.ebo = 0;
        other.vertexCount = other.indexCount = 0;
    }

    /*!****************************************************************************
     * \brief Move assignment operator
     *
     * This is used when existing object is already instanciated, but we are moving
     * another object's data into it.
     *
     * \param other GeometryBuffer object to be moved from.
     * \return \b GeometryBuffer& this
     *****************************************************************************/
    GeometryBuffer& GeometryBuffer::operator=(GeometryBuffer&& other) noexcept {
        if ( this != &other ) {
            // Clean up current object's old buffers
            cleanupBuffers();

            vao = other.vao;
            vbo = other.vbo;
            ebo = other.ebo;
            vertexCount = other.vertexCount;
            indexCount = other.indexCount;

            // Reset other
            other.vao = other.vbo = other.ebo = 0;
            other.indexCount = other.vertexCount = 0;
        }

        return *this;
    }

    /*!****************************************************************************
     * \brief Bind the VAO, making it the current AO for draw calls
     *
     *****************************************************************************/
    void GeometryBuffer::bind() const {
        if ( vao != boundVAO ) {
            glBindVertexArray(vao);
            boundVAO = vao;
        }
    }

    /*!****************************************************************************
     * \brief Unbind the VAO, resetting the current AO to 0
     *
     *****************************************************************************/
    void GeometryBuffer::unbind() const {
        glBindVertexArray(0);
    }

    // This is for if or when we do dynamic typing
    // size_t get_sizeof_gltype(GLenum type) {
    //  switch (type) {
    //  case GL_BYTE:
    //  case GL_UNSIGNED_BYTE:
    //    return sizeof(GLbyte);
    //
    //  case GL_SHORT:
    //  case GL_UNSIGNED_SHORT:
    //    return sizeof(GLshort);
    //
    //  case GL_INT_2_10_10_10_REV:
    //  case GL_INT:
    //  case GL_UNSIGNED_INT_2_10_10_10_REV:
    //  case GL_UNSIGNED_INT:
    //    return sizeof(GLint);
    //
    //  case GL_FLOAT:
    //    return sizeof(GLfloat);
    //
    //  case GL_DOUBLE:
    //    return sizeof(GLdouble);
    //
    //  case GL_FIXED:
    //    return sizeof(GLfixed);
    //
    //  case GL_HALF_FLOAT:
    //    return sizeof(GLhalf);
    //  }
    //}

    /*!****************************************************************************
     * \brief Initialize the VBOs with the attributeData
     *
     * \param attributeData Attributes object that contains the VBO info.
     *****************************************************************************/
    void GeometryBuffer::initializeVertexBuffers(Attributes& attributeData) {

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // When data is given in interleaved format.
        if ( isInterleaved ) {
            GLsizeiptr totalSize = 0;
            GLsizeiptr offset = 0;
            for ( const auto& [type, info] : attributeData ) {
                attributeOffsets[type] = offset;
                offset += static_cast<GLsizeiptr>(info.elementSize * sizeof(float));
                totalSize += info.data.size() * sizeof(float);
                bufferAttributeData[type] = info;
            }

            auto it = attributeData.at(AttributeType::Position);
            vertexCount = static_cast<GLsizei>(it.data.size() / it.elementSize);

            // When in interleaved format, only the first attribute needs to have data.
            // Position is assumed to be the first attribute, which has the data.
            glBufferData(GL_ARRAY_BUFFER, totalSize, it.data.data(), GL_STATIC_DRAW);

            GLuint index = 0;
            for ( const auto& [type, info] : attributeData ) {
                glVertexAttribPointer(
                    index,
                    info.elementSize,
                    info.type,
                    info.normalized,
                    info.stride,
                    (void*)attributeOffsets[type]);

                glEnableVertexAttribArray(index);
                index++;
            }
        }
        // When data is given in block format
        else {
            GLsizeiptr totalSize = 0;
            for ( const auto& [type, info] : attributeData ) {
                attributeOffsets[type] = totalSize;
                totalSize += (info.data.size() * sizeof(float));
                bufferAttributeData[type] = info;
            }

            auto it = attributeData.begin();
            vertexCount = static_cast<GLsizei>(it->second.data.size() / it->second.elementSize);

            glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

            /**
             * We are storing the data as blocks of each attribute. We are not using the
             * interleaved data approach.
             */
            for ( const auto& [type, info] : attributeData ) {
                glBufferSubData(
                    GL_ARRAY_BUFFER,
                    attributeOffsets[type],
                    info.data.size() * sizeof(float),
                    info.data.data());

                glVertexAttribPointer(
                    static_cast<GLuint>(type),
                    info.elementSize,
                    info.type,
                    info.normalized,
                    info.elementSize * sizeof(float),
                    (void*)attributeOffsets[type]);

                glEnableVertexAttribArray(static_cast<GLuint>(type));
            }
        }
    }

    /*!****************************************************************************
     * \brief Initialize the EBO with the indices
     *
     * \param indices The indices to be used for the EBO.
     *****************************************************************************/
    void GeometryBuffer::initializeElementBuffers(
        const std::vector<unsigned int>& indices) {

        indexCount = (unsigned int)indices.size();

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);

        this->indexData = indices;
    }

    /*!****************************************************************************
     * \brief Initialize the Buffers
     *
     * We are creating the VAO and VBO here.
     * We are storing the vertexes and color as blocks instead of interleaved.
     * Each block has all the vectors next to each other with no extra stride.
     * (Stride = sizeof(vector))
     *
     * \param attributeData Attribute data for each attribute. Refer to Attribute
     *  type for more info.
     *****************************************************************************/
    void GeometryBuffer::initializeBuffers(
        Attributes& attributeData) {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        initializeVertexBuffers(attributeData);

        glBindVertexArray(0);
    }

    /*!****************************************************************************
     * \brief Initialize the Buffers
     *
     * We are creating the VAO, VBO and EBO here.
     * We are storing the vertexes and color as blocks instead of interleaved.
     * Each block has all the vectors next to each other with no extra stride.
     * (Stride = sizeof(vector))
     *
     * \param attributeData Attribute data for each attribute. Refer to Attribute
     *  type for more info.
     * \param indices
     *****************************************************************************/
    void GeometryBuffer::initializeBuffers(
        Attributes& attributeData,
        const std::vector<unsigned int>& indices) {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        initializeVertexBuffers(attributeData);
        initializeElementBuffers(indices);

        glBindVertexArray(0);
    }

    /*!****************************************************************************
     * \brief Update the vertex attributes used in the VBO
     *
     * ## Usage:
     *
     * This is only to be used if the VBO data is in block format.
     *
     * DO NOT USE THIS CALL WHEN DATA IS INTERLEAVED
     *
     * \param type Type of attribute to be modified.
     * \param data Data of the attribute to be updated.
     *****************************************************************************/
    void GeometryBuffer::updateVertexAttribute(
        const AttributeType& type,
        const std::vector<float>& data) {

        if ( isInterleaved ) {
            throw std::runtime_error("Invalid Attribute update");
        }

        if ( !hasAttribute(type) ) {
            throw std::runtime_error("Attribute not found in buffer");
        }
        glBindVertexArray(vao);

        glBufferSubData(
            GL_ARRAY_BUFFER,
            attributeOffsets[type],
            data.size() * sizeof(float),
            data.data());

        glBindVertexArray(0);
    }

    /*!****************************************************************************
     * \brief Update the EBO indices
     *
     * \param indices Indices to be used in the EBO.
     *****************************************************************************/
    void GeometryBuffer::updateIndices(const std::vector<unsigned int>& indices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        indexCount = static_cast<GLsizei>(indices.size());
        this->indexData = indices;
    }

    /*!****************************************************************************
     * \brief If an attribute has been set in the VBOs
     *
     * \param type Attrinbute type.
     * \return \b bool True if attribute exists in VBO.
     *****************************************************************************/
    bool GeometryBuffer::hasAttribute(AttributeType type) {
        return attributeOffsets.find(type) != attributeOffsets.end();
    }

    /*!****************************************************************************
     * \brief Delete all VAO, VBO and EBO
     *
     *****************************************************************************/
    void GeometryBuffer::cleanupBuffers() {
        if ( vao != 0 ) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }
        if ( vbo != 0 ) {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }
        if ( ebo != 0 ) {
            glDeleteBuffers(1, &ebo);
            ebo = 0;
        }
    }

} // namespace LaurelEye::Graphics
