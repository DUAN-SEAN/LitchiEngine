
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
file(GLOB_RECURSE HEADER_FILES "*.h" "*.inl")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${rttr_cpp})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${GLAD_FILES})

# 添加目标库
# add_library(${TARGET_NAME} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES})
# add_library(${TARGET_NAME} ${rttr_cpp} ${GLAD_FILES} ${glfw_sources} ${HEADER_FILES} ${SOURCE_FILES} ${imgui_src})
add_library(${TARGET_NAME} ${rttr_cpp} ${GLAD_FILES} ${HEADER_FILES} ${SOURCE_FILES})

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
target_compile_options(${TARGET_NAME} 
	PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->"
	PRIVATE 
	$<$<CXX_COMPILER_ID:MSVC>:/bigobj> 
	$<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)
	
# Link dependencies    
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC stb)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/mono/Libs)
target_link_libraries(${TARGET_NAME} PUBLIC mono-2.0-sgen)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/assimp/lib)
target_link_libraries(${TARGET_NAME} PUBLIC assimp-vc142-mt)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/freetype-windows-binaries-master/static/vs2015-2022/win64)
target_link_libraries(${TARGET_NAME} PUBLIC freetype)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32)
target_link_libraries(${TARGET_NAME} PUBLIC vulkan-1)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/dxc/lib)
target_link_libraries(${TARGET_NAME} PUBLIC dxcompiler)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/spirv_cross/lib)
target_link_libraries(${TARGET_NAME} PUBLIC spirv-cross-core_debug spirv-cross-cpp_debug spirv-cross-glsl_debug spirv-cross-hlsl_debug)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/compressonator/lib)
target_link_libraries(${TARGET_NAME} PUBLIC Compressonator_MT_debug)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/free_image/lib)
target_link_libraries(${TARGET_NAME} PUBLIC FreeImageLib_debug)

target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/easy_profiler_core/lib)
target_link_libraries(${TARGET_NAME} PUBLIC easy_profiler)

# 将Source目录添加到Link Target 的项目include中
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/Source>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rttr>)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glm-0.9.9.8/glm>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glad/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glfw-3.3.8>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rapidxml>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rapidjson-1.1.0>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/mono/Include/mono-2.0>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/assimp/include>)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype/include> PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype/devel>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype-windows-binaries-master/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/VulkanSDK/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/vulkanmemoryallocator/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/spirv_cross>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/dxc/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/compressonator>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/free_image/FreeImage>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/easy_profiler_core/include>)