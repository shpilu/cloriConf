
find_path(ZOOKEEPER_INCLUDE_PATH
    zookeeper/zookeeper.h
    )

find_library(ZOOKEEPER_LIBRARY NAMES zookeeper_mt)

if(ZOOKEEPER_INCLUDE_PATH AND ZOOKEEPER_LIBRARY)
    set(ZOOKEEPER_FOUND TRUE)
endif()
if(ZOOKEEPER_FOUND)
    message(STATUS "Found zookeeper: ${ZOOKEEPER_INCLUDE_PATH}")
else()
    message(FATAL_ERROR "Could not find zookeeper...")
endif()
