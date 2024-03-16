set(THIRD_PARTY_DIR "ThirdParty(new)")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# 添加第三方库
add_subdirectory(assimp)
# add_subdirectory(compressonator)
# add_subdirectory(dxcompiler)
add_subdirectory(easy_profiler)
add_subdirectory(FreeImage)
# add_subdirectory(freetype)
add_subdirectory(glfw)
# add_subdirectory(mono)
# add_subdirectory(PhysX)
add_subdirectory(spdlog)
# add_subdirectory(spirv_cross)
add_subdirectory(stb)
# add_subdirectory(vulkan)