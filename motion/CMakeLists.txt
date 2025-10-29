add_library(motion
    motion.c
    motion.h
)

target_include_directories(motion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(motion PUBLIC planner kinematics utils)
