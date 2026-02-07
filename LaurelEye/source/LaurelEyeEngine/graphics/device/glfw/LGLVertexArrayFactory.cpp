#include "LaurelEyeEngine/graphics/device/glfw/LGLVertexArrayFactory.h"

#include "LaurelEyeEngine/graphics/resources/VertexArray.h"

namespace LaurelEye::Graphics {

    LGLVertexArrayFactory::~LGLVertexArrayFactory() {
        destroyAll();
    }

    VertexArrayHandle LGLVertexArrayFactory::createArray(const VertexArrayDesc& d) {
        LGLVertexArrayRecord r{};
        r.desc = d;

        glCreateVertexArrays(1, &r.id);

        // 1. Set up vertex buffer bindings
        for ( const auto& vb : d.vertexBindings ) {
            glVertexArrayVertexBuffer(
                r.id,
                vb.bindingIndex,
                vb.buffer,
                static_cast<GLintptr>(vb.offset),
                static_cast<GLsizei>(vb.stride));
        }

        // 2. Setup attributes
        auto toGLFormat = [](VertexAttribFormat fmt, GLint& size, GLenum& type, GLboolean& normalized) {
            normalized = GL_FALSE;
            switch ( fmt ) {
            case VertexAttribFormat::Float32x2:
                size = 2;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::Float32x3:
                size = 3;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::Float32x4:
                size = 4;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::Uint32x4Norm:
                size = 4;
                type = GL_UNSIGNED_INT;
                normalized = GL_TRUE;
                break;
            case VertexAttribFormat::Uint32x4:
                size = 4;
                type = GL_UNSIGNED_INT;
                break;
            case VertexAttribFormat::Int32x4:
                size = 4;
                type = GL_FLOAT;
                break;
            case VertexAttribFormat::Float32:
                size = 1;
                type = GL_FLOAT;
                break;
            default:
                // handle error / assert
                size = 4;
                type = GL_FLOAT;
                break;
            }
        };

        for ( const auto& attr : d.attributes ) {
            GLint size;
            GLenum type;
            GLboolean normalized;
            toGLFormat(attr.format, size, type, normalized);

            glEnableVertexArrayAttrib(r.id, attr.location);

            //if ( attr.format == VertexAttribFormat::Uint8x4 || attr.format == VertexAttribFormat::Int8x4 )
            //{
            //    glVertexArrayAttribIFormat( r.id,
            //        (GLint)attr.location,
            //        size,
            //        type,
            //        // GL_UNSIGNED_INT , GLINT
            //        (GLuint)attr.relativeOffset);
            //}
            //else {
                glVertexArrayAttribFormat(
                    r.id,
                    (GLint)attr.location,
                    size,
                    type,
                    normalized,
                    (GLuint)attr.relativeOffset);
            //}
            

            glVertexArrayAttribBinding(
                r.id,
                attr.location,
                attr.bindingIndex);
        }

        // 3. Index buffer (optional)
        if ( isValidDataBuffer(d.indexBuffer) ) {
            glVertexArrayElementBuffer(r.id, d.indexBuffer);
        }

        // Store VAO and return handle
        createdArrays[r.id] = r;
        return r.id;
    }

    void LGLVertexArrayFactory::destroyArray(VertexArrayHandle h) {
        glDeleteVertexArrays(1, &createdArrays[h].id);
        createdArrays.erase(h);
    }

    void LGLVertexArrayFactory::destroyAll() {
        for ( auto& [h, d] : createdArrays ) {
            glDeleteBuffers(1, &createdArrays[h].id);
        }
        createdArrays.clear();
    }

    void LGLVertexArrayFactory::bindBase(VertexArrayHandle h) {
        const LGLVertexArrayRecord& r = createdArrays[h];
        glBindVertexArray(r.id);
    }

} // namespace LaurelEye::Graphics
