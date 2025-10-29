add_library(osal
    osal.h
    osal_posix.c
)

target_include_directories(osal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(osal PUBLIC pthread)
