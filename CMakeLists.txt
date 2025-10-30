cmake_minimum_required(VERSION 3.20)
project(delta_cnc_studio LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_BRACHISTO "Enable brachistochrone planner" ON)
option(BUILD_GUI "Build Qt-based IDE" ON)

if(ENABLE_BRACHISTO)
    add_compile_definitions(ENABLE_BRACHISTO=1)
else()
    add_compile_definitions(ENABLE_BRACHISTO=0)
endif()

add_library(cnc_core STATIC
    core/cnc_api.c
    planner/splines/splines.c
    utils/q16.c
    utils/vec3.c)

target_include_directories(cnc_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/planner
        ${CMAKE_CURRENT_SOURCE_DIR}/utils)

target_compile_options(cnc_core PRIVATE -Wall -Wextra -Werror)

if(BUILD_GUI)
    find_package(Qt6 COMPONENTS Widgets OpenGL OpenGLWidgets REQUIRED)
    qt_standard_project_setup()

    set(GUI_SOURCES
        adapters/trajectory_adapter.cpp
        gui/MainWindow.cpp
        gui/main.cpp
        gui/editors/SplineEditor.cpp
        gui/view3d/TrajectoryView.cpp
        project/project_manager.cpp
        render/opengl_renderer.cpp)

    qt_add_executable(baget_ide
        MANUAL_FINALIZATION
        ${GUI_SOURCES})

    target_include_directories(baget_ide PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/adapters
        ${CMAKE_CURRENT_SOURCE_DIR}/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/editors
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/view3d
        ${CMAKE_CURRENT_SOURCE_DIR}/project
        ${CMAKE_CURRENT_SOURCE_DIR}/render)

    target_link_libraries(baget_ide PRIVATE
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        cnc_core)

    qt_finalize_executable(baget_ide)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/api)
    add_custom_target(docs_api
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation")
endif()

find_package(Sphinx)
if(Sphinx_FOUND)
    add_custom_target(docs
        COMMAND ${SPHINX_EXECUTABLE} -b html ${CMAKE_CURRENT_SOURCE_DIR}/docs/source ${CMAKE_CURRENT_BINARY_DIR}/docs/html
        COMMAND ${SPHINX_EXECUTABLE} -b latex ${CMAKE_CURRENT_SOURCE_DIR}/docs/source ${CMAKE_CURRENT_BINARY_DIR}/docs/latex
        COMMENT "Building Sphinx documentation")
endif()
