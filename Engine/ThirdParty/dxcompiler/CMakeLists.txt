cmake_minimum_required(VERSION 3.21)

# Project name and a few useful settings. Other commands can pick up the results
project(dxcompiler_root VERSION 1.0 DESCRIPTION "DXC Compiler Project")

include(${CMAKE_CURRENT_LIST_DIR}/cmake/build.cmake)

add_subdirectory(source)