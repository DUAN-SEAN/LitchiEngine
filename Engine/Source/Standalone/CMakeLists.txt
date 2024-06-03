set(TARGET_NAME LitchiStandalone)

file(GLOB_RECURSE STANDALONE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
file(GLOB_RECURSE STANDALONE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${STANDALONE_HEADERS} ${STANDALONE_SOURCES})
		
# 显示包含PhysX静态库
# include(../../ThridParty/physx-4.1/CMakeLists.txt)

# 添加执行程序
add_executable(${TARGET_NAME} ${STANDALONE_HEADERS} ${STANDALONE_SOURCES} ${STANDALONE_RESOURCE})

add_compile_definitions("Litchi_ROOT_DIR=${BINARY_ROOT_DIR}")
target_compile_options(${TARGET_NAME} 
	PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->"
	PRIVATE 
	$<$<CXX_COMPILER_ID:MSVC>:/bigobj> 
	$<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)

# 设置相关编译选项
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "LitchiStandalone")
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                                               VS_DEBUGGER_COMMAND           "$<TARGET_FILE:${TARGET_NAME}>"
                                               VS_DEBUGGER_ENVIRONMENT       "PATH=%PATH%;${CMAKE_PREFIX_PATH}/bin")
# if (MSVC)
    # set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# else()
    # add_definitions(-D _DEBUG)
# endif ()

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

# set(POST_BUILD_COMMANDS
  # COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy_directory "../../Assets/" "${BINARY_ROOT_DIR}/../Assets/"
# )

# add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		#set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
        file(GLOB MODEL ${PHYSX_DEBUG_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Debug/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Debug/*.dll)
    else()
		#set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
        file(GLOB MODEL ${PHYSX_RELEASE_LIB_PATH}/*.dll ${THIRD_PARTY_DIR}/dxcompiler/lib/Release/*.dll ${THIRD_PARTY_DIR}/assimp/bin/Release/*.dll)
    endif()
	
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MODEL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
	#file(GLOB DATA ${ENGINE_ASSET_DIR})
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${ENGINE_ASSET_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/Data)
endif()