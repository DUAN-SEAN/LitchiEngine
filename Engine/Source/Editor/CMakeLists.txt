
set(TARGET_NAME LitchiEditor)

# 递归地从当前源目录的include子目录中获取所有.h和.inl文件
file(GLOB_RECURSE EDITOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
# 递归地从当前源目录的source子目录中获取所有.cpp文件
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
		
# 将EDITOR_HEADERS和EDITOR_SOURCES文件添加到源组
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES})
	
# 根据不同的构建模式添加特定的宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
	add_compile_definitions(PX_PHYSX_STATIC_LIB)
endif()

# 添加执行程序
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

# PhysX
set(PHYSX_DEBUG_LIB_PATH ${THIRD_PARTY_DIR}/PhysX/lib/debug)
set(PHYSX_RELEASE_LIB_PATH ${THIRD_PARTY_DIR}/PhysX/lib/release)
target_link_directories(${TARGET_NAME}
	PUBLIC
    $<$<CONFIG:Debug>:${PHYSX_DEBUG_LIB_PATH}>
    $<$<CONFIG:Release>:${PHYSX_RELEASE_LIB_PATH}>
)

# 链接LitchiRuntime和PhysX库
target_link_libraries(${TARGET_NAME} LitchiRuntime PhysX_64 PhysXCommon_64 PhysXCooking_64 PhysXExtensions_static_64 PhysXFoundation_64 PhysXPvdSDK_static_64)

# 如果是MSVC环境，复制一些.dll文件到目标文件目录
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
        file(GLOB MODEL ${PHYSX_DEBUG_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Debug/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Debug/*.dll)
    else()
		set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
        file(GLOB MODEL ${PHYSX_RELEASE_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Release/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Release/*.dll)
    endif()


	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MODEL} $<TARGET_FILE_DIR:${TARGET_NAME}>)

	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSET_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/Data)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_CONFIG_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/Config)
endif()
