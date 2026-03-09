# Find all subdirectories (each should contain its own CMakeLists.txt)
file(GLOB GAME_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

include(LaurelEyeGame)

foreach(GAME_DIR ${GAME_DIRS})
    if(LAURELEYE_BUILD_GAMES AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GAME_DIR})
        message(STATUS "Configuring game project: ${GAME_DIR}")
        add_subdirectory(${GAME_DIR})
    endif()
endforeach()
