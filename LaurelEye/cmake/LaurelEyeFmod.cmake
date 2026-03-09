function(laureleye_add_fmod out_target)
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

    add_library(${out_target} SHARED IMPORTED GLOBAL)

    if(WIN32)

        set_target_properties(${out_target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${FMOD_INCLUDE_DIR}"
            IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo;MinSizeRel"

            IMPORTED_IMPLIB_DEBUG "${FMOD_DEBUG_IMPLIB}"
            IMPORTED_LOCATION_DEBUG "${FMOD_DEBUG_DLL}"

            IMPORTED_IMPLIB_RELEASE "${FMOD_RELEASE_IMPLIB}"
            IMPORTED_LOCATION_RELEASE "${FMOD_RELEASE_DLL}"

            IMPORTED_IMPLIB_RELWITHDEBINFO "${FMOD_RELEASE_IMPLIB}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${FMOD_RELEASE_DLL}"

            IMPORTED_IMPLIB_MINSIZEREL "${FMOD_RELEASE_IMPLIB}"
            IMPORTED_LOCATION_MINSIZEREL "${FMOD_RELEASE_DLL}"
        )
    else()
        set_target_properties(${out_target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${FMOD_INCLUDE_DIR}"
            IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo;MinSizeRel"

            IMPORTED_LOCATION_DEBUG "${FMOD_DEBUG_SO}"

            IMPORTED_LOCATION_RELEASE "${FMOD_RELEASE_SO}"
            IMPORTED_LOCATION_RELWITHDEBINFO "${FMOD_RELEASE_SO}"
            IMPORTED_LOCATION_MINSIZEREL "${FMOD_RELEASE_SO}"
        )
    endif()
endfunction()
