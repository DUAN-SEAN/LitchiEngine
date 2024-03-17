# add_definitions(-DDISABLE_FORCE_DEBUG_POSTFIX)

set(ENGINE_ROOT_DIR "${LITCHI_ROOT_DIR}/Engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/ThirdParty(new)")
set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/Asset")

# option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# 如果是MSVC环境，设置编译选项为MP,并且设置某个项目为启动项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # add_compile_options("/MTd")
    # add_compile_options("/MP")
	# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LitchiEditor)
endif()

# 如果没有指定构建类型，那么默认为Release
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_INSTALL_PREFIX Release CACHE STRING "Build Type" FORCE)
endif()

# 根据构建类型设置编译选项
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    # 在Debug模式下使用多线程调试DLL (/MTd)
    add_compile_options("/MTd")
    # 启用并行编译
    add_compile_options("/MP")
elseif (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    # 在Release模式下使用多线程DLL (/MT)
	add_compile_options("/MT")
        # 启用并行编译
    add_compile_options("/MP")
else()
	message("else:${CMAKE_BUILD_TYPE}")
	message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    add_compile_options("/MP")
endif()

# 在Debug模式下定义_DEBUG宏
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# 如果是Windows环境并且不是Cygwin，那么在Debug模式下定义_DEBUG宏
# IF (WIN32 AND NOT CYGWIN)
#     SET(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
# ENDIF ()

# 平台相关设置
#if(WIN32)
#elseif(UNIX)
#    if(APPLE)
#	else()
#	endif()
#else()
#endif()

link_directories(${CMAKE_BINARY_DIR}/bin)

# 添加第三方构建子目录
add_subdirectory("ThirdParty(new)")

# 添加运行时构建子目录
add_subdirectory(Source/Runtime)

# 添加引擎编辑器构建子目录
add_subdirectory(Source/Editor)

# 添加引擎脚本构建子目录
add_subdirectory(Source/ScriptCore)
