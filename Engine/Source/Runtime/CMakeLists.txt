
set(TARGET_NAME LitchiRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 引入cmakeList 临时写法
# include(CMakeLists.txt.glfw)
include(../../ThridParty/physx-4.1/CMakeLists.txt)

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE rttr_cpp "../../ThridParty/rttr/*.cpp")
file(GLOB_RECURSE GLAD_FILES "../../ThridParty/glad/*.c")
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
set(imgui_src ${THIRD_PARTY_DIR}/imgui/imgui.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp)

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${rttr_cpp})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${GLAD_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${imgui_src})

# 添加目标库
# add_library(${TARGET_NAME} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES})
add_library(${TARGET_NAME} ${rttr_cpp} ${GLAD_FILES} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES} ${imgui_src})

# 设置Target工程 C++17版本
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
# 将Target工程添加到目录Engine目录下 
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# if (MSVC)
    # set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# else()
    # add_definitions(-D _DEBUG)
# endif ()

# being a cross-platform target, we enforce standards conformance on MSVC
# 这个配置会导致rttr编译报错
# target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PRIVATE stb)

# 将Source目录添加到Link Target 的项目include中
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/Source>)

# include rttr 
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rttr>)

# include glm
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glm-release/glm>)

# include glad
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glad/include>)

# include glfw
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glfw-3.3.8>)

# include rapidxml
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rapidxml>)

# include stb
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>)

# include stb
target_include_directories(${TARGET_NAME} 
PUBLIC 
  $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/imgui> ${THIRD_PARTY_DIR}/imgui/backends)
