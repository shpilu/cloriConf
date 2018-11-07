#
# cloriConf cmake 
# require cmake >= 2.8.10 && gcc >= 4.8
#
cmake_minimum_required(VERSION 2.8.10)
project(cloriconf CXX)

set(CLORICONF_VERSION 1.0.0)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC is too old, please install a newer version supporting C++11")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with GCC only")
endif()

add_subdirectory(src)

