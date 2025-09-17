#include "LaurelEyeEngine/core/Vector3.h"
#include "LaurelEyeEngine/core/VectorTemplated.h"
#include "LaurelEyeEngine/core/Matrix4.h"

int main() {
    std::cout << "Hello CMake." << std::endl;

    LaurelEye::Vector3 vec1 = { 1.0, 2.0, 3.0 };
    LaurelEye::Vector3 vec2 = { 3.0, 2.0, 1.0 };

    std::cout << (
        ( vec1 + vec2 == LaurelEye::Vector3(4.0, 4.0, 4.0) ) ? "Passed" : "Failed"
        ) << std::endl;



    LaurelEye::Vector4 vec3 = LaurelEye::Vector4();
    vec3[0] = 1.0f;
    vec3[1] = 2.0f;
    vec3[2] = 3.0f;
    vec3[3] = 4.0f;
    LaurelEye::Vector4 vec4 = LaurelEye::Vector4();
    vec4[0] = 4.0f;
    vec4[1] = 3.0f;
    vec4[2] = 2.0f;
    vec4[3] = 1.0f;

    std::cout << (
        ( vec3 + vec4 == LaurelEye::Vector4(5.0) ) ? "Passed" : "Failed"
        ) << std::endl;

    LaurelEye::Matrix4 mat1 = LaurelEye::Matrix4();

    std::cout << mat1 << std::endl;

    return 0;
}
