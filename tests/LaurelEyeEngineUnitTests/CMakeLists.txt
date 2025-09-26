# CMakeList.txt : CMake project for LaurelEyeEngine, include source and define
# project specific logic here.
#

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(LaurelEyeEngineTest
    ${TEST_SOURCES}
)

target_link_libraries(LaurelEyeEngineTest
    PRIVATE
        LaurelEyeEngine
)

target_include_directories(LaurelEyeEngineTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET LaurelEyeEngineTest PROPERTY CXX_STANDARD 23)
endif()

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# TODO: Add tests and install targets if needed.
