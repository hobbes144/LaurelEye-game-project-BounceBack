/*!****************************************************************************
 * \file   RendererStateSaver.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \date   02-08-2025
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/graphics/RenderStateSaver.h"
#include "LaurelEyeEngine/graphics/device/IRenderDevice.h"

namespace LaurelEye::Graphics {

    /* StateSaver Class */
    /*!****************************************************************************
     * \brief Constructor for StateSaver
     *
     * ## Explanation:
     *
     * This will save the state of the renderer when created.
     *
     * \param renderer
     *****************************************************************************/
    RenderStateSaver::RenderStateSaver(IRenderDevice* _rd) : rd(_rd), savedState(_rd->getCurrentState()) {};

    /*!****************************************************************************
     * \brief Destructor for StateSaver
     *
     * ## Explanation:
     *
     * This will restore the state of the renderer to the state it was in when
     * this object was created.
     *
     *****************************************************************************/
    RenderStateSaver::~RenderStateSaver() {
        rd->setState(savedState);
    }

} // namespace LaurelEye::Graphics
