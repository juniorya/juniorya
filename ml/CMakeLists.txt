add_library(ml STATIC
    tinyml_optimizer.c
)

target_include_directories(ml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ..)

target_link_libraries(ml PUBLIC utils)

set_target_properties(ml PROPERTIES POSITION_INDEPENDENT_CODE ON)
