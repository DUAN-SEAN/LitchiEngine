set(ENGINE_ROOT_DIR "${LITCHI_ROOT_DIR}/Engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/ThirdParty")
set(ENGINE_ASSET_DIR "${ENGINE_ROOT_DIR}/Data")
set(ENGINE_CONFIG_DIR "${ENGINE_ROOT_DIR}/Config")

# option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# 如果是MSVC环境，设置编译选项为MP,并且设置某个项目为启动项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LitchiEditor)
endif()

    #set(CMAKE_BUILD_TYPE "Release")
# 如果没有指定构建类型，那么默认为Release
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_INSTALL_PREFIX Release CACHE STRING "Build Type" FORCE)
endif()

# 仅对 Visual Studio 编译器生效
if(MSVC)
    # 启用并行编译
    add_compile_options("/MP")

    # 根据构建类型设置编译选项
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # 在Debug模式下使用多线程调试DLL (/MTd)
        add_compile_options("/MTd")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        # 在Release模式下使用多线程DLL (/MT)
        add_compile_options("/MT")
    else()
        message("Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# 将库文件输出目录作为链接目录
link_directories(${CMAKE_BINARY_DIR}/bin)

# 添加第三方构建子目录
add_subdirectory(ThirdParty)

# 添加运行时构建子目录
add_subdirectory(Source/Runtime)

# 添加引擎编辑器构建子目录
add_subdirectory(Source/Editor)

# 添加引擎脚本构建子目录
add_subdirectory(Source/ScriptCore)

# 添加桌面Standalone构建子目录
add_subdirectory(Source/Standalone)
