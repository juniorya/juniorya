add_library(kinematics
    delta_kinematics.c
    delta_kinematics.h
)

target_include_directories(kinematics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(kinematics PUBLIC utils)
