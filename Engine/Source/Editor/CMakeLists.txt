set(TARGET_NAME LitchiEditor)

file(GLOB_RECURSE EDITOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl)
file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)


# dest build/****/Assets 和Editor目录平级
file(COPY "../../Assets/" DESTINATION "Debug/Project/Assets/")


# file(GLOB_RECURSE imgui_src "../../ThridParty/imgui/backends/*.cpp")

# set(imgui_src ${THIRD_PARTY_DIR}/imgui/imgui.cpp
        # ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
        # ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
        # ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
        # ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
		# ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
		# ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp)
		
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES})
# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}../../../" FILES ${imgui_src})
		

# 显示包含PhysX静态库
include(../../ThridParty/physx-4.1/CMakeLists.txt)

# 添加执行程序
# add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE} ${imgui_src})
add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

add_compile_definitions("Litchi_ROOT_DIR=${BINARY_ROOT_DIR}")
target_compile_options(${TARGET_NAME} 
	PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->"
	PRIVATE 
	$<$<CXX_COMPILER_ID:MSVC>:/bigobj> 
	$<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>)
	
# 添加stb头文件
# target_include_directories(
  # ${TARGET_NAME} 
  # PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>
# )

# 设置相关编译选项
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "LitchiEditor")
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
endif()