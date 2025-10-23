
project(SampleGame LANGUAGES CXX)

# Collect all .cpp files for the game
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Build as executable (not library)
add_executable(SampleGame ${GAME_SOURCES})

# Include directories (same as engine)
target_include_directories(SampleGame
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link the game against the engine
target_link_libraries(SampleGame
    PRIVATE
        LaurelEyeEngine
)

# Set C++ standard like the engine
if (CMAKE_VERSION VERSION_GREATER 3.20)
    set_property(TARGET SampleGame PROPERTY CXX_STANDARD 23)
endif()

# Optional: place it in a "Games" folder in Visual Studio Solution Explorer
set_target_properties(SampleGame PROPERTIES
    FOLDER "Games"
)
