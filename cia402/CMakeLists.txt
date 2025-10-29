add_library(cia402
    cia402.c
    cia402.h
)

target_include_directories(cia402 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(cia402 PUBLIC utils)
