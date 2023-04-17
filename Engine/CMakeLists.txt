
set(ENGINE_ROOT_DIR "${LITCHI_ROOT_DIR}/Engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/ThridParty")
set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/Asset")

# option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# 如果是MSVC环境，设置编译选项为MP,并且设置某个项目为启动项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # add_compile_options("/MTd")
    add_compile_options("/MP")
	# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LitchiEditor)
endif()

# 平台相关设置
#if(WIN32)
#elseif(UNIX)
#    if(APPLE)
#	else()
#	endif()
#else()
#endif()

# 添加第三方构建子目录
add_subdirectory(ThridParty)

# 添加运行时构建子目录
add_subdirectory(Source/Runtime)

# 添加引擎编辑器构建子目录
add_subdirectory(Source/Editor)
