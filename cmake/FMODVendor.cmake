# cmake/FMODVendor.cmake
function(le_add_fmod TARGET_NAME)
    set(options USE_STUDIO USE_LOGGING) # USE_LOGGING => link L variants
    cmake_parse_arguments(FMOD "${options}" "" "" ${ARGN})

    set(FMOD_ROOT "${CMAKE_SOURCE_DIR}/external/fmod")
    set(FMOD_INC  "${FMOD_ROOT}/include")

    # Choose core + (optional) studio library names per config
    if (MSVC)
        set(_core_dbg   "${FMOD_ROOT}/windows/fmodL_vc.lib")
        set(_core_rel   "${FMOD_ROOT}/windows/fmod_vc.lib")
        set(_studio_dbg "${FMOD_ROOT}/windows/fmodstudioL_vc.lib")
        set(_studio_rel "${FMOD_ROOT}/windows/fmodstudio_vc.lib")

        set(_core_dll_dbg   "${FMOD_ROOT}/windows/fmodL.dll")
        set(_core_dll_rel   "${FMOD_ROOT}/windows/fmod.dll")
        set(_studio_dll_dbg "${FMOD_ROOT}/windows/fmodstudioL.dll")
        set(_studio_dll_rel "${FMOD_ROOT}/windows/fmodstudio.dll")
    elseif (UNIX AND NOT APPLE)
        set(_core_dbg   "${FMOD_ROOT}/linux/x86_64/libfmodL.so")
        set(_core_rel   "${FMOD_ROOT}/linux/x86_64/libfmod.so")
        set(_studio_dbg "${FMOD_ROOT}/linux/x86_64/libfmodstudioL.so")
        set(_studio_rel "${FMOD_ROOT}/linux/x86_64/libfmodstudio.so")
    else()
        message(FATAL_ERROR "FMODVendor: Unsupported platform")
    endif()

    # Configure per-configuration library paths
    if (FMOD_USE_LOGGING)
        set(_core_lib "$<$<CONFIG:Debug>:${_core_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_core_rel}>")
        set(_studio_lib "$<$<CONFIG:Debug>:${_studio_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_studio_rel}>")
    else()
        set(_core_lib "$<$<CONFIG:Debug>:${_core_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_core_rel}>")
        set(_studio_lib "$<$<CONFIG:Debug>:${_studio_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_studio_rel}>")
        # If you prefer *always* release in non-Debug: already handled above
    endif()

    # Link + include
    target_include_directories(${TARGET_NAME} PUBLIC "${FMOD_INC}")
    if (MSVC)
        target_link_libraries(${TARGET_NAME} PUBLIC "${_core_lib}")
        if (FMOD_USE_STUDIO)
            target_link_libraries(${TARGET_NAME} PUBLIC "${_studio_lib}")
        endif()
        # Copy runtime DLLs next to the target so it launches in the build tree
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<$<CONFIG:Debug>:${_core_dll_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_core_dll_rel}>
            $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        if (FMOD_USE_STUDIO)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<$<CONFIG:Debug>:${_studio_dll_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_studio_dll_rel}>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        endif()
    else() # Linux
        # Link directly to .so; set rpath so loader finds them beside the exe
        target_link_libraries(${TARGET_NAME} PUBLIC "${_core_lib}")
        if (FMOD_USE_STUDIO)
            target_link_libraries(${TARGET_NAME} PUBLIC "${_studio_lib}")
        endif()
        set_target_properties(${TARGET_NAME} PROPERTIES
            BUILD_RPATH "$ORIGIN"
            INSTALL_RPATH "$ORIGIN"
        )
        # Optional: copy .so next to the binary in build tree
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<$<CONFIG:Debug>:${_core_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_core_rel}>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        if (FMOD_USE_STUDIO)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<$<CONFIG:Debug>:${_studio_dbg}>$<$<NOT:$<CONFIG:Debug>>:${_studio_rel}>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        endif()
    endif()
endfunction()

