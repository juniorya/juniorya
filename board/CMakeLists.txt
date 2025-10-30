add_library(board
    board.c
    board.h
)

target_include_directories(board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
