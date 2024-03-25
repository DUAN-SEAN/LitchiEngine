
set(TARGET_NAME LitchiRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 引入cmakeList 临时写法
# include(CMakeLists.txt.glfw)
# include(../../ThirdParty/physx-4.1/CMakeLists.txt)

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE rttr_cpp "../../ThirdParty/rttr/*.cpp")
file(GLOB_RECURSE GLAD_FILES "../../ThirdParty/glad/*.c")
file(GLOB_RECURSE HEADER_FILES "*.h" "*.inl")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#source_group(TREE "${ENGINE_ROOT_DIR}/schema" FILES ${SCHEMA_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${rttr_cpp})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${GLAD_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
endif()

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
target_link_libraries(${TARGET_NAME} PUBLIC spdlog)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC stb)

target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/mono/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/mono/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC libmono-static-sgen)

# assimp
target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/assimp/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/assimp/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC assimp-vc142-mt)

# freetype
target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/freetype/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/freetype/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC freetype)

# Physx
target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/PhysX/lib/win.x86_64.vc143.mt/debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/PhysX/lib/win.x86_64.vc143.mt/release>
)
target_link_libraries(${TARGET_NAME} PUBLIC PhysX_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64)

# foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
#     if(CONFIG_TYPE STREQUAL "Debug")
#         target_link_libraries(${TARGET_NAME} PUBLIC
#             PhysX_64 
#             PhysXFoundation_64 
#             PhysXExtensions_static_64 
#             PhysXPvdSDK_static_64)
#     else()
#         target_link_libraries(${TARGET_NAME} PUBLIC
#             glut32 
#             PhysXDevice64)
#     endif()
# endforeach()

# Vulkan
target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32)
target_link_libraries(${TARGET_NAME} PUBLIC vulkan-1)

# DxCompiler
target_link_directories(${TARGET_NAME}
    PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/dxcompiler/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/dxcompiler/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC libdxcompiler)

target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/spirv_cross/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/spirv_cross/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC spirv-cross-core spirv-cross-cpp spirv-cross-glsl spirv-cross-hlsl)

target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/compressonator/lib/Debug>
    $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/compressonator/lib/Release>
)
target_link_libraries(${TARGET_NAME} PUBLIC compressonator)

# target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/free_image/lib)
target_link_libraries(${TARGET_NAME} PUBLIC FreeImage)

# target_link_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/easy_profiler_core/lib)
target_link_libraries(${TARGET_NAME} PUBLIC easy_profiler)

# 将Source目录添加到Link Target 的项目include中
target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/PhysX/physx/include)
# target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/PhysX/pxshared/include)
# include_directories(${THIRD_PARTY_DIR}/PhysX/physx/include)
# include_directories(${THIRD_PARTY_DIR}/PhysX/pxshared/include)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/PhysX/physx/include)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/PhysX/pxshared/include)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/Source>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rttr>)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glm-0.9.9.8/glm>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glad/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/glfw/include>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rapidxml>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/rapidjson>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/mono/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/assimp/include>)
# target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype/include> PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype/devel>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/freetype/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/VulkanSDK/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/vulkanmemoryallocator/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/spirv_cross/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/dxcompiler/include/dxc>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/compressonator/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/FreeImage/FreeImage>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/easy_profiler/include>)