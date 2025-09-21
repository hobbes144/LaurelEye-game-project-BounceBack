/*!****************************************************************************
 * \file   EngineMath.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   04-18-2025
 *
 * Copyright � 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/core/EngineMath.h"

#include <random>

namespace LaurelEye {

    namespace EngineMath {

        float clamp(float value, float min, float max) {
            return (value < min) ? min : ((value > max) ? max : value);
        }

        // Random
        namespace Random {

            float boundedFloat(float min, float max) {
                static std::mt19937 gen([] {
                    std::random_device rd;
                    return std::mt19937(rd());
                }());

                std::uniform_real_distribution<float> dist(min, max);
                return dist(gen);
            }

        } // namespace Random

    } // namespace EngineMath

} // namespace LaurelEye
