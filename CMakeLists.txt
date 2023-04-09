cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Litchi VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# 设置相关路径
set(LITCHI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# 添加子构建目录
add_subdirectory(Engine)


