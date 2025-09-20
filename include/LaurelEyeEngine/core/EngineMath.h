/*!****************************************************************************
 * \file   EngineMath.h
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \date   03-13-2025
 * \brief  Main Engine Math class to provide a central Maths library
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

#pragma once

#include <random>

namespace LaurelEye {

    namespace EngineMath {

        float clamp(float value, float min, float max);

        namespace Random {
            float boundedFloat(float min, float max);
        }

    } // namespace EngineMath

} // namespace LaurelEye

#endif // !ENGINE_MATH_H
