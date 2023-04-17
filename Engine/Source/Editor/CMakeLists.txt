set(TARGET_NAME LitchiEditor)

# dest build/****/Assets 和Editor目录平级
file(COPY "../../Assets/" DESTINATION "../Assets/")

file(GLOB EDITOR_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB EDITOR_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES})

# 显示包含PhysX静态库
include(../../ThridParty/physx-4.1/CMakeLists.txt)

# 添加执行程序
add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

add_compile_definitions("Litchi_ROOT_DIR=${BINARY_ROOT_DIR}")

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

#set(POST_BUILD_COMMANDS
  # COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resource" "${BINARY_ROOT_DIR}/resource"
  # COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${BINARY_ROOT_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEPLOY_CONFIG_DIR}/${TARGET_NAME}.ini" "${BINARY_ROOT_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEVELOP_CONFIG_DIR}/${TARGET_NAME}.ini" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
  # COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"
  # COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ROOT_DIR}/${ENGINE_ASSET_DIR}" "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"
#)



# if(ENABLE_PHYSICS_DEBUG_RENDERER)
  # set(POST_BUILD_COMMANDS ${POST_BUILD_COMMANDS}
    # COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${JOLT_ASSET_DIR}"
    # COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ROOT_DIR}/${JOLT_ASSET_DIR}" "${BINARY_ROOT_DIR}/${JOLT_ASSET_DIR}"
# )
# endif()

# add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})

# #precompile
# #set global vari used by precompile
# set(Litchi_EDITOR_HEADS “${EDITOR_HEADERS}” PARENT_SCOPE)
