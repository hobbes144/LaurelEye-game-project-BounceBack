#pragma once

#include "LaurelEyeEngine/graphics/resources/VertexArray.h"

#include <glad/glad.h>

#include <unordered_map>

namespace LaurelEye::Graphics {

    class LGLDataBufferFactory;

    struct LGLVertexArrayRecord {
        VertexArrayDesc desc{};

        GLuint id = 0;
    };

    class LGLVertexArrayFactory {
    public:
        ~LGLVertexArrayFactory();

        [[nodiscard]] VertexArrayHandle createArray(const VertexArrayDesc& d);

        void destroyArray(VertexArrayHandle h);

        void destroyAll();

        /// \copydoc IRenderDevice::bindDataBufferBase
        void bindBase(VertexArrayHandle h);

    private:
        std::unordered_map<VertexArrayHandle, LGLVertexArrayRecord> createdArrays;
    };

} // namespace LaurelEye::Graphics
