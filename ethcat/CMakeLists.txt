add_library(ethcat
    ethcat.c
    ethcat.h
    xml_loader.c
    xml_loader.h
)

target_include_directories(ethcat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(ethcat PUBLIC utils osal)
