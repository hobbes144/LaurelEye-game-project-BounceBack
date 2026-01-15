if(BUILD_DOCS)

    find_package(Doxygen REQUIRED doxygen)

    set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/doc/")
    set(DOXYGEN_EXTRACT_ALL         YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")

    doxygen_add_docs(
        api-docs
        ${CMAKE_SOURCE_DIR}/source
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/README.md
        ${CMAKE_SOURCE_DIR}/coding_conventions.md
        ${CMAKE_SOURCE_DIR}/feature_branch_workflow.md
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMENT "Generate HTML documentation"
        # USE_STAMP_FILE  # uncomment to speed up incremental builds
    )

endif()
