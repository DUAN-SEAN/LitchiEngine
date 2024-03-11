cmake_minimum_required(VERSION 3.10)
project(Glad)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE GLAD_SRC
     "${PROJECT_SOURCE_DIR}/include/glad/glad.h"
     "${PROJECT_SOURCE_DIR}/include/KHR/khrplatform.h"
     "${PROJECT_SOURCE_DIR}/src/glad.c")

add_library(Glad STATIC ${GLAD_SRC})

target_include_directories(Glad PRIVATE "${PROJECT_SOURCE_DIR}/include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Glad PRIVATE HZ_DEBUG)
    set_target_properties(Glad PROPERTIES DEBUG_POSTFIX d)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Glad PRIVATE HZ_RELEASE)
endif()