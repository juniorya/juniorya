add_library(ide_support STATIC
    ide_project.c
)

target_include_directories(ide_support
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ide_support
    PUBLIC
        utils
        planner
        sim_support
        storage
        ml
        ethcat
        vision
)

if(TARGET_OS STREQUAL "host" OR TARGET_OS STREQUAL "qemu")
    add_executable(cnc_ide
        ide_app.c
    )
    target_link_libraries(cnc_ide
        PRIVATE
            ide_support
            sim_support
            utils
            planner
            storage
            ethcat
    )
    set_target_properties(cnc_ide PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ide)
endif()
