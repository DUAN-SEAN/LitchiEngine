set(TARGET_NAME LitchiStandalone)

file(GLOB_RECURSE STANDALONE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
file(GLOB_RECURSE STANDALONE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${STANDALONE_HEADERS} ${STANDALONE_SOURCES})
		
# 显示包含PhysX静态库
include(../../ThridParty/physx-4.1/CMakeLists.txt)

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

# if (MSVC)
    # set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# else()
    # add_definitions(-D _DEBUG)
# endif ()

# 此步骤将引用LitchiRuntime提供给外界的include，以及其Link 目标的include以及库
target_link_libraries(${TARGET_NAME} LitchiRuntime)

# set(POST_BUILD_COMMANDS
  # COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy_directory "../../Assets/" "${BINARY_ROOT_DIR}/../Assets/"
# )

# add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})


if(MSVC)
	file(GLOB MODEL ${THIRD_PARTY_DIR}/mono/Libs/*.dll ${THIRD_PARTY_DIR}/assimp/bin/*.dll ${THIRD_PARTY_DIR}/physx-4.1/physx/bin/win.x86_64.vc142.mt/debug/*.dll ${THIRD_PARTY_DIR}/easy_profiler_core/lib/*.dll)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MODEL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
	#file(GLOB DATA ${ENGINE_ASSET_DIR})
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ENGINE_ASSET_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/Data)
endif()