
set(TARGET_NAME LitchiRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 示例，添加某个目录下的子工程 
#set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)
#add_library(json11 ${JSON_INCLUDE}/json11.cpp)
#set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

# 添加头文件
include_directories("../../ThridParty/rttr")
include_directories("../../ThridParty/rapidxml")
include_directories("../../ThridParty/glm-release/glm")
# include_directories("../../ThridParty/glfw/deps")

# 引入cmakeList
include(CMakeLists.txt.glfw)
include(../../ThridParty/physx-4.1/CMakeLists.txt)

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE rttr_cpp "../../ThridParty/rttr/*.cpp")
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../ThridParty/rttr/" FILES ${rttr_cpp})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../ThridParty/" FILES ${glfw_sources})

# 添加目标库
# add_library(${TARGET_NAME} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES})
add_library(${TARGET_NAME} ${rttr_cpp} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES})

# 设置Target工程 C++17版本
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
# 将Target工程添加到目录Engine目录下 
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# being a cross-platform target, we enforce standards conformance on MSVC
# 这个配置会导致rttr编译报错
# target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# Link dependencies    
# target_link_libraries(${TARGET_NAME} PUBLIC imgui)
# target_link_libraries(${TARGET_NAME} PUBLIC glm)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PRIVATE stb)
target_link_libraries(${TARGET_NAME} PRIVATE rttr)
# target_link_libraries(${TARGET_NAME} PUBLIC glfw) # 不需要在这里link了，直接加入编译
# target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})
# target_link_libraries(${TARGET_NAME} PUBLIC physX)

#if(ENABLE_PHYSICS_DEBUG_RENDERER)
#  add_compile_definitions(ENABLE_PHYSICS_DEBUG_RENDERER)
#  target_link_libraries(${TARGET_NAME} PUBLIC TestFramework d3d12.lib shcore.lib)
#endif()

# 将Source目录添加到Link Target 的项目include中
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/Source>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)


