/// @file   LGLDataBufferFactory.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of DataBufferFactory for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLDataBufferFactory.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"

#include <GL/gl.h>
#include <GL/glext.h>
#include <stdexcept>

namespace LaurelEye::Graphics {

    LGLDataBufferFactory::~LGLDataBufferFactory() {
        destroyAll();
    }

    DataBufferHandle LGLDataBufferFactory::createBuffer(const DataBufferDesc& d, const void* initialData, const std::string& debugName) {
        LGLDataBufferRecord r{};
        r.desc = d;

        GLbitfield flags = 0;
        if ( d.mode == DataBufferUpdateMode::Dynamic ) {
            flags = GL_DYNAMIC_STORAGE_BIT;
        }
        else if ( d.mode == DataBufferUpdateMode::Static ) {
            flags = 0;
        }
        else {
            throw std::runtime_error("LAURELEYE::GRAPHICS::OPENGL::DATABUFFERFACTORY::Persistent buffers unimplemented");
        }

        // Create the buffer
        glCreateBuffers(1, &r.id);
        glNamedBufferStorage(r.id, (GLsizeiptr)d.sizeBytes, initialData, flags);

#if !defined(NDEBUG)
        std::cout << "Created buffer: " << debugName << std::endl;
        glObjectLabel(GL_BUFFER, r.id, -1, debugName.c_str());
#endif

        DataBufferHandle h = r.id;

        createdBuffers[h] = r;

        // Setup ring if persistent here
        // This includes call to glbindBufferRange, which sets up which part
        // of the data range is visible per shader run. This sets up the ring.

        // Separated in case there's any type specific init needed later.
        switch ( d.type ) {
        case DataBufferType::UBO:
            glBindBufferBase(GL_UNIFORM_BUFFER, d.bindingPoint, r.id);
            break;
        case DataBufferType::SSBO:
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, d.bindingPoint, r.id);
            break;
        case DataBufferType::Vertex:
            // Binding is deferred to VAO creation via glVertexArrayVertexBuffer(...)
            break;
        case DataBufferType::Index:
            // Binding is deferred to VAO creation via glVertexArrayElementBuffer(...)
            break;
        }

        return h;
    }

    void LGLDataBufferFactory::destroyBuffer(DataBufferHandle h) {

        // Unmap persistent buffer:
        // glUnmapNamedBuffer(r->id);

        glDeleteBuffers(1, &createdBuffers[h].id);
        createdBuffers.erase(h);
    }

    void LGLDataBufferFactory::destroyAll() {
        for ( auto& [h, d] : createdBuffers ) {
            glDeleteBuffers(1, &createdBuffers[h].id);
        }
        createdBuffers.clear();
    }

    void LGLDataBufferFactory::updateSubData(DataBufferHandle h, size_t offset, size_t size, const void* data) {
        glNamedBufferSubData(h, (GLintptr)offset, (GLsizeiptr)size, data);
    }

    // This is for resize, we don't do this right now so skipping.
    // void LGLDataBufferFactory::updateWholeData(DataBufferHandle h, uint64_t size, const void* data) {
    //     glNamedBufferData(h, size, data, GL_DYNAMIC_STORAGE_BIT);
    // }

    void LGLDataBufferFactory::bindBase(DataBufferHandle h) {
        const LGLDataBufferRecord& r = createdBuffers[h];
        if ( r.desc.type == DataBufferType::UBO ) {
            glBindBufferBase(GL_UNIFORM_BUFFER, r.desc.bindingPoint, r.id);
        }
        else {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, r.desc.bindingPoint, r.id);
        }
    }

} // namespace LaurelEye::Graphics
