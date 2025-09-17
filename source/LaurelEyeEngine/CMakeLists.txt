# Engine library
# project (LaurelEyeEngine VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Engine")

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiled.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
)

add_library(LaurelEyeEngine SHARED ${ENGINE_SOURCES})

# add_library (LaurelEyeEngine
#   SHARED
#   "LaurelEyeEngine.cpp"
# )

target_precompile_headers(LaurelEyeEngine
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/LaurelEyeEngine/pch.hpp
#   PRIVATE
#     [["other_header.h"]]
#     <unordered_map>
)

# Public headers
target_include_directories(LaurelEyeEngine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(LaurelEyeEngine PROPERTIES PUBLIC_HEADER include/LaurelEyeEngine/LaurelEyeEngine.h)

if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET LaurelEyeEngine PROPERTY CXX_STANDARD 23)
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
