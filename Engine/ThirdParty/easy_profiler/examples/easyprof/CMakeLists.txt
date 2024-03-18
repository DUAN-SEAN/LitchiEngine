cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(easyprof)

set(CMAKE_CXX_STANDARD 17)

add_executable(easyprof main.cpp)
find_package(glog CONFIG REQUIRED)
target_link_libraries(easyprof PRIVATE glog::glog)
target_include_directories(easyprof PRIVATE "/opt/vcpkg/installed/x64-osx/include")
target_link_directories(easyprof PRIVATE "/opt/vcpkg/installed/x64-osx/debug/lib")
target_link_libraries(easyprof PUBLIC easy_profiler)
