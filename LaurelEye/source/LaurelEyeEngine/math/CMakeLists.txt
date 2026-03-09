add_library(LaurelEyeMath STATIC)

add_library(LaurelEye::Math ALIAS LaurelEyeMath)

target_sources(LaurelEyeMath
    PRIVATE
        ArcLengthTable.cpp
        CatmullRomSpline.cpp
        EngineMath.cpp
        Matrix4.cpp
        Quaternion.cpp
        Spline.cpp
        Transform.cpp
        VectorTemplated.cpp
        Vector3.cpp
)

target_precompile_headers(LaurelEyeMath
    PRIVATE
        ${LAURELEYE_ROOT_DIR}/include/LaurelEyeEngine/pch.hpp
)

target_include_directories(LaurelEyeMath
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(LaurelEyeMath
    PUBLIC
        ${RAPIDJSON_INCLUDE_DIRS}
)

target_compile_definitions(LaurelEyeMath
    PRIVATE
        LAURELEYE_MATH_BUILD
)

set_target_properties(LaurelEyeMath PROPERTIES
    OUTPUT_NAME "LaurelEyeMath"
    FOLDER "Engine"
)
