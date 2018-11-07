
find_path(RAPIDJSON_INCLUDE_PATH
    rapidjson/document.h
    )
if(RAPIDJSON_INCLUDE_PATH)
    set(RAPIDJSON_FOUND TRUE)
endif()
if(RAPIDJSON_FOUND)
    message(STATUS "Found rapidjson: ${RAPIDJSON_INCLUDE_PATH}")
else()
    message(FATAL_ERROR "Could not find rapidjson cpp header files")
endif()
