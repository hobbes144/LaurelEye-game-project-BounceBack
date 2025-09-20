find_package(Doxygen REQUIRED doxygen)

set (DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/doc/")

doxygen_add_docs(doc "doc/" "source/" "tests/" "include/" ALL
                 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                 COMMENT "Generate HTML documentation")
