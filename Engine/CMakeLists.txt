
set(ENGINE_ROOT_DIR "${LITCHI_ROOT_DIR}/Engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/ThridParty")
set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/Asset")

# option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# 如果是MSVC环境，设置编译选项为MP,并且设置某个项目为启动项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # add_compile_options("/MTd")
    # add_compile_options("/MP")
	# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LitchiEditor)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_INSTALL_PREFIX Release CACHE STRING "Build Type" FORCE)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_compile_options("/MTd")
    add_compile_options("/MP")
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
	add_compile_options("/MT")
    add_compile_options("/MP")
else()
	message("else:${CMAKE_BUILD_TYPE}")
	message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

IF (WIN32 AND NOT CYGWIN)
    SET(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
ENDIF ()

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

# 添加引擎脚本构建子目录
add_subdirectory(Source/ScriptCore)
