set(TARGET_NAME LitchiEditor)

# 递归地从当前源目录的include子目录中获取所有.h和.inl文件
file(GLOB_RECURSE EDITOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
# 递归地从当前源目录的source子目录中获取所有.cpp文件
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# dest build/****/Assets 和Editor目录平级
# 将Assets目录复制到Debug/Project/Assets目录
# file(COPY "../../Assets/" DESTINATION "Debug/Project/Assets/")
		
# 将EDITOR_HEADERS和EDITOR_SOURCES文件添加到源组
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES})
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${imgui_src})
		
# 显示包含PhysX静态库
# include(../../ThirdParty/PhysX/CMakeLists.txt)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     add_definitions(-D_DEBUG)
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#     add_definitions(-DNDEBUG)
# endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
endif()

# 添加执行程序
# add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE} ${imgui_src})
add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

# 添加编译定义
add_compile_definitions("Litchi_ROOT_DIR=${BINARY_ROOT_DIR}")
target_compile_options(${TARGET_NAME} 
	PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->"
	PRIVATE 
	$<$<CXX_COMPILER_ID:MSVC>:/bigobj> 
	$<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)

# 设置相关编译选项
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "LitchiEditor")
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# Physx
set(PHYSX_DEBUG_LIB_PATH ${THIRD_PARTY_DIR}/PhysX/lib/win.x86_64.vc143.mt/debug)
set(PHYSX_RELEASE_LIB_PATH ${THIRD_PARTY_DIR}/PhysX/lib/win.x86_64.vc143.mt/release)
target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${PHYSX_DEBUG_LIB_PATH}>
    $<$<CONFIG:Release>:${PHYSX_RELEASE_LIB_PATH}>
)

# 此步骤将引用LitchiRuntime提供给外界的include，以及其Link 目标的include以及库
# target_link_libraries(${TARGET_NAME} LitchiRuntime)
target_link_libraries(${TARGET_NAME} PUBLIC LitchiRuntime PhysX_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64)

# foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
# 	if(CONFIG_TYPE STREQUAL "Debug")
#         target_link_libraries(${TARGET_NAME} PUBLIC
# 			LitchiRuntime
#             PhysX_64 
#             PhysXFoundation_64 
#             PhysXExtensions_static_64 
#             PhysXPvdSDK_static_64)
#     else()
#         target_link_libraries(${TARGET_NAME} PUBLIC
# 			LitchiRuntime
#             glut32 
#             PhysXDevice64)
#     endif()
# endforeach()

# 如果是MSVC环境，复制一些.dll文件到目标文件目录
# todo 根据不同的Build Type复制不同的文件
if(MSVC)
	# file(GLOB MODEL ${THIRD_PARTY_DIR}/mono/libs/*.dll ${THIRD_PARTY_DIR}/assimp/bin/*.dll ${THIRD_PARTY_DIR}/PhysX/physx/bin/win.x86_64.vc142.mt/debug/*.dll ${THIRD_PARTY_DIR}/easy_profiler_core/lib/*.dll)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(GLOB MODEL ${PHYSX_DEBUG_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Debug/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Debug/*.dll)
    else()
        file(GLOB MODEL ${PHYSX_RELEASE_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Release/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Release/*.dll)
    endif()

	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MODEL} $<TARGET_FILE_DIR:${TARGET_NAME}>)

	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSET_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/Project/Assets)
endif()