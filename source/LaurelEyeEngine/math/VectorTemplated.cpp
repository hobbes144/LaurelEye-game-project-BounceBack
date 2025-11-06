// VectorTemplated.cpp
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye {

    // Force these instantiations to be generated ONCE:
    template class VectorTemplated<int, 2>;
    template class VectorTemplated<float, 2>;
    template class VectorTemplated<double, 2>;
    template class VectorTemplated<int, 3>;
    template class VectorTemplated<double, 3>;
    template class VectorTemplated<int, 4>;
    template class VectorTemplated<float, 4>;
    template class VectorTemplated<double, 4>;
} // namespace LaurelEye
