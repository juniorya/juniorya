set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_SYSTEM_PROCESSOR mips64)
# Expect QNX SDP environment variables to provide qcc
if(NOT DEFINED ENV{QNX_HOST})
    message(FATAL_ERROR "QNX toolchain requires QNX_HOST")
endif()
set(CMAKE_C_COMPILER "$ENV{QNX_HOST}/usr/bin/qcc")
set(CMAKE_C_COMPILER_ARG1 "-Vgcc_nto64")
