
if(WITH_DEBUG_SYMBOLS)
    SET(DEBUG_SYMBOL "-g")
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/def.h.in ${PROJECT_BINARY_DIR}/output/include/internal/def.h @ONLY)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
if(ENABLE_JSON)
    find_package(RAPIDJSON REQUIRED)
endif()
if(ENABLE_ZOOKEEPER)
    find_package(ZOOKEEPER REQUIRED)
endif()

include_directories(${CMAKE_CURRENT_SRC_DIR}
    ${PROJECT_BINARY_DIR}/output/include)

set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} ${DEBUG_SYMBOL}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -O2 -pipe -Wall -W -fPIC -fstrict-aliasing -Wno-invalid-offsetof -Wno-unused-parameter -fno-omit-frame-pointer")

macro(use_cxx11)
if(CMAKE_VERSION VERSION_LESS "3.1.3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
endmacro(use_cxx11)

use_cxx11()

set(DYNAMIC_LIB
    pthread
    dl)

if(ENABLE_JSON)
    include_directories(${RAPIDJSON_INCLUDE_PATH})
    set(DYNAMIC_LIB "${DYNAMIC_LIB} ${RAPIDJSON_LIB}")
endif()
if(ENABLE_ZOOKEEPER)
    include_directories(${ZOOKEEPER_INCLUDE_PATH})
    set(DYNAMIC_LIB "${DYNAMIC_LIB} ${ZOOKEEPER_LIB}")
    set(CLORICONF_PRIVATE_LIBS "${CLORICONF_PRIVATE_LIBS} -lzookeeper_mt") 
endif()

# for *.so output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output/lib)
# for *.a output
set(CMAKE_ARCHIVE_OUTPUT_DIRECOTRY ${PROJECT_BINARY_DIR}/output/lib)

file(GLOB BASIC_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cc")
set(PACKAGE_SOURCES ${BASIC_SOURCES} 
    ${PROJECT_SOURCE_DIR}/src/loader/config_keeper.cc
    ${PROJECT_SOURCE_DIR}/src/loader/joml.cc
    ${PROJECT_SOURCE_DIR}/src/loader/direct/config_direct.cc
    ${PROJECT_SOURCE_DIR}/src/loader/local/config_local.cc)
if(ENABLE_JSON)
    set(PACKAGE_SOURCES ${PACKAGE_SOURCES}
        ${PROJECT_SOURCE_DIR/src/loader/json/json.cc)
endif()

if(ENABLE_ZOOKEEPER)
    set(PACKAGE_SOURCES ${PACKAGE_SOURCES}
        ${PROJECT_SOURCE_DIR/src/loader/zookeeper/config_zk.cc)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(OBJ_LIB OBJECT ${PACKAGE_SOURCES})
set_property(TARGET ${OBJ_LIB} PROPERTY POSITION_INDEPENTENT_CODE 1)
add_library(cloriconf-shared SHARED $<TARGET_OBJECTS:OBJ_LIB>)
add_library(cloriconf-static STATIC $<TARGET_OBJECTS:OBJ_LIB>)

target_link_libraries(cloriconf-shared ${DYNAMIC_LIB})
set_target_properties(cloriconf-shared PROPERTIES OUTPUT_NAME cloriconf CLEAN_DIRECT_OUTPUT 1)
set_target_properties(cloriconf-static PROPERTIES OUTPUT_NAME cloriconf CLEAN_DIRECT_OUTPUT 1)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)
set(TUTORIAL_SOURCES ${PROJECT_SOURCE_DIR}/src/example/tutorial.cc)
add_executable(tutorial ${TUTORIAL_SOURCES})
target_link_libraries(tutorial cloriconf-shared)

file(COPY ${PROJECT_SOURCE_DIR}/bin/
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})

file(COPY ${PROJECT_SOURCE_DIR}/conf/
    DESTINATION ${PROJECT_BINARY_DIR}/output/conf)

file(COPY ${PROJECT_SOURCE_DIR}/src/config.h DESTINATION ${PROJECT_BINARY_DIR}/output/include/) 
file(COPY ${PROJECT_SOURCE_DIR}/src/internal/
    DESTINATION ${PROJECT_BINARY_DIR}/output/include/internal/
    FILES_MATCHING
    PATTERN "*.h"
    ) 
configure_file(${PROJECT_SOURCE_DIR}/cmake/cloriconf.pc.in ${PROJECT_BINARY_DIR}/output/cloriconf.pc)

# install
install(TARGETS cloriconf-shared 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS cloriconf-static
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${PROJECT_BINARY_DIR}/output/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING
    PATTERN "*.h"
    )

install(FILES ${PROJECT_BINARY_DIR}/output/cloriconf.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
