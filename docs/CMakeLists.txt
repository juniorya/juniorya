find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen not found; docs_api target will be unavailable")
endif()

find_program(SPHINX_BUILD sphinx-build)
if(NOT SPHINX_BUILD)
    message(WARNING "sphinx-build not found; docs target will be unavailable")
endif()

if(DOXYGEN_FOUND)
    add_custom_target(docs_api
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs/api
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating Doxygen API reference"
    )
endif()

if(SPHINX_BUILD)
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E env SPHINX_APIDOC_OPTIONS=members
                ${SPHINX_BUILD} -b html ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_BINARY_DIR}/docs/html
        COMMAND ${SPHINX_BUILD} -b latex ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_BINARY_DIR}/docs/latex
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/docs/latex make
        DEPENDS docs_api
        COMMENT "Building full documentation set"
    )
endif()
