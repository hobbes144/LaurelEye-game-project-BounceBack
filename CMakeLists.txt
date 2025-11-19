# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(
        CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
endif()

project ("GAM55X-Laurel" VERSION 0.0.1 DESCRIPTION "Team Laurel GAM55x Project")

set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# if (MSVC)
#     add_compile_options(/W4 /WX)
# else()
#     add_compile_options(-Wall -Wextra -pedantic -Werror)
# endif()


# Output directories
set(OUTPUT_ROOT "${CMAKE_SOURCE_DIR}/build/output")
set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/build/intermediate/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/build/intermediate/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
)
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/build/output/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
  "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
)

# Include sub-projects.
add_subdirectory("source/LaurelEyeEngine")
add_subdirectory("tests/LaurelEyeEngineUnitTests")
add_subdirectory("games")

option(BUILD_DOCS "Build API documentation" OFF)
add_subdirectory("doc")
