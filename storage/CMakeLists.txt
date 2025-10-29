add_library(storage
    storage.c
    storage.h
)

target_include_directories(storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(storage PUBLIC utils)
