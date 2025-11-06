/*!****************************************************************************
 * \file   RendererStateSaver.h
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \date   02-08-2025
 * \brief  State Saver class for the Renderer
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderState.h"

namespace LaurelEye::Graphics {

    class IRenderDevice;

    class RenderStateSaver {
    public:
        explicit RenderStateSaver(IRenderDevice* _rd);
        ~RenderStateSaver();

        // Disable moving
        RenderStateSaver(const RenderStateSaver&) = delete;
        RenderStateSaver& operator=(const RenderStateSaver&) = delete;

    private:
        // Currently not used, but needed for later.
        IRenderDevice* rd;
        RenderState savedState;
    };

} // namespace LaurelEye::Graphics
