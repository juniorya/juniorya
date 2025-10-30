add_library(opcua
    opcua.c
    opcua.h
)

target_include_directories(opcua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(opcua PUBLIC utils)
