function(laureleye_add_game GAME_NAME)
    set(options)
    set(oneValueArgs ASSETS_DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(GAME "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(GAME_OUTPUT_DIR "${LAURELEYE_OUTPUT_DIR}/${GAME_NAME}")

    # Game directory (this folder)
    set(GAME_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    if(NOT GAME_SOURCES)
        message(FATAL_ERROR "laureleye_add_game(${GAME_NAME}) requires SOURCES.")
    endif()

    add_executable(${GAME_NAME}
        ${GAME_SOURCES}
    )

    target_link_libraries(${GAME_NAME}
        PRIVATE
            LaurelEye::Engine
    )

    target_include_directories(${GAME_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
    )

    set_target_properties(${GAME_NAME} PROPERTIES
        FOLDER "Games/${GAME_NAME}"

        RUNTIME_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${GAME_OUTPUT_DIR}"

        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GAME_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GAME_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GAME_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${GAME_OUTPUT_DIR}"
    )

    # Install the executable
    install(TARGETS ${GAME_NAME}
        RUNTIME DESTINATION ${GAME_NAME}
        LIBRARY DESTINATION ${GAME_NAME}
        BUNDLE DESTINATION ${GAME_NAME}
    )
    install(RUNTIME_DEPENDENCY_SET ${GAME_NAME}_runtime_deps)

    # Install detected runtime dependencies (DLLs/.so)
    install(RUNTIME_DEPENDENCY_SET ${GAME_NAME}_runtime_deps
        DESTINATION ${GAME_NAME}
        PRE_EXCLUDE_REGEXES
            "api-ms-.*"
            "ext-ms-.*"
        POST_EXCLUDE_REGEXES
            ".*/system32/.*\\.dll"
            ".*/Windows/.*"
            ".*/usr/lib/.*"
            ".*/lib/x86_64-linux-gnu/.*"
    )

    add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${GAME_NAME}>/assets"
    )
    add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${GAME_NAME}>/../../../assets"
    )
    add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${GAME_NAME}>/assets"
    )
    add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${GAME_NAME}>/../../../assets"
    )

    # Install assets next to the game if provided
    if(GAME_ASSETS_DIR AND EXISTS "${GAME_ASSETS_DIR}")
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${GAME_ASSETS_DIR}"
                "$<TARGET_FILE_DIR:${GAME_NAME}>/assets"
        )
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${GAME_ASSETS_DIR}"
                "$<TARGET_FILE_DIR:${GAME_NAME}>/../../../assets"
        )
        install(DIRECTORY "${GAME_ASSETS_DIR}/"
            DESTINATION "${GAME_NAME}/assets"
        )
    endif()

    # FMOD runtime
    set(FMOD_ROOT "${CMAKE_SOURCE_DIR}/external/fmod")
    if(WIN32)
        set(FMOD_INCLUDE_DIR "${FMOD_ROOT}/include")
        set(FMOD_LIB_DIR     "${FMOD_ROOT}/windows")
        set(FMOD_BIN_DIR     "${FMOD_ROOT}/windows")

        set(FMOD_RELEASE_IMPLIB "${FMOD_LIB_DIR}/fmod_vc.lib")
        set(FMOD_DEBUG_IMPLIB   "${FMOD_LIB_DIR}/fmodL_vc.lib")

        set(FMOD_RELEASE_DLL    "${FMOD_LIB_DIR}/fmod.dll")
        set(FMOD_DEBUG_DLL      "${FMOD_LIB_DIR}/fmodL.dll")
    elseif(UNIX)
        set(FMOD_INCLUDE_DIR "${FMOD_ROOT}/include")
        set(FMOD_LIB_DIR     "${FMOD_ROOT}/linux/x86_64")

        set(FMOD_RELEASE_SO "${FMOD_LIB_DIR}/libfmod.so")
        set(FMOD_DEBUG_SO   "${FMOD_LIB_DIR}/libfmodL.so")
    else()
        message(FATAL_ERROR "Unsupported platform for FMOD.")
    endif()
    if(WIN32)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<IF:$<CONFIG:Debug>,${FMOD_DEBUG_DLL},${FMOD_RELEASE_DLL}>"
                "$<TARGET_FILE_DIR:${GAME_NAME}>/$<IF:$<CONFIG:Debug>,fmodL.dll,fmod.dll>"
            VERBATIM
        )

        install(FILES
            "$<$<CONFIG:Debug>:${FMOD_DEBUG_DLL}>"
            "$<$<NOT:$<CONFIG:Debug>>:${FMOD_RELEASE_DLL}>"
            DESTINATION "${GAME_NAME}"
        )
    elseif(UNIX)
        add_custom_command(TARGET ${GAME_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<IF:$<CONFIG:Debug>,${FMOD_DEBUG_SO},${FMOD_RELEASE_SO}>"
                "$<TARGET_FILE_DIR:${GAME_NAME}>/$<IF:$<CONFIG:Debug>,libfmodL.so,libfmod.so>"
            VERBATIM
        )

        install(FILES
            "$<$<CONFIG:Debug>:${FMOD_DEBUG_SO}>"
            "$<$<NOT:$<CONFIG:Debug>>:${FMOD_RELEASE_SO}>"
            DESTINATION "${GAME_NAME}"
        )
    endif()

endfunction()
