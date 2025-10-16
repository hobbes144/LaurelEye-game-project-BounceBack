/// @file   LGLDataBufferFactory.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of DataBufferFactory for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLDataBufferFactory.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"

#include <stdexcept>

namespace LaurelEye::Graphics {

    DataBufferHandle LGLDataBufferFactory::createBuffer(const DataBufferDesc& d, const void* initialData) {
        LGLDataBufferRecord r{};
        r.desc = d;

        GLbitfield flags = 0;
        if ( d.mode == DataBufferUpdateMode::Dynamic ) {
            flags = GL_DYNAMIC_STORAGE_BIT;
        }
        else {
            throw std::runtime_error("LAURELEYE::GRAPHICS::OPENGL::DATABUFFERFACTORY::Persistent buffers unimplemented");
        }

        // Create the buffer
        glCreateBuffers(1, &r.id);
        glNamedBufferStorage(r.id, (GLsizeiptr)d.sizeBytes, initialData, flags);

        DataBufferHandle h = r.id;

        createdBuffers[h] = r;

        // Setup ring if persistent here
        // This includes call to glbindBufferRange, which sets up which part
        // of the data range is visible per shader run. This sets up the ring.

        // Separated in case there's any type specific init needed later.
        if ( d.type == DataBufferType::UBO ) {
            glBindBufferBase(GL_UNIFORM_BUFFER, d.bindingPoint, r.id);
        }
        else {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, d.bindingPoint, r.id);
        }

        return h;
    }

    void LGLDataBufferFactory::destroyBuffer(DataBufferHandle h) {

        // Unmap persistent buffer:
        // glUnmapNamedBuffer(r->id);

        glDeleteBuffers(1, &createdBuffers[h].id);
        createdBuffers.erase(h);
    }

    void LGLDataBufferFactory::updateSubData(DataBufferHandle h, uint64_t offset, uint64_t size, const void* data) {
        glNamedBufferSubData(h, offset, size, data);
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
