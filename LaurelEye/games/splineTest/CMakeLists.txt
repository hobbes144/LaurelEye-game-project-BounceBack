
project(AnimationTesting LANGUAGES CXX)

# Collect all .cpp files for the game
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as executable (not library)
add_executable(AnimationTesting ${GAME_SOURCES})

# Include directories (same as engine)
target_include_directories(AnimationTesting
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link the game against the engine
target_link_libraries(AnimationTesting
    PRIVATE
        LaurelEyeEngine
)

# Set C++ standard like the engine
if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET AnimationTesting PROPERTY CXX_STANDARD 23)
endif()

# Optional: place it in a "Games" folder in Visual Studio Solution Explorer
set_target_properties(AnimationTesting PROPERTIES
    FOLDER "Games"
)
