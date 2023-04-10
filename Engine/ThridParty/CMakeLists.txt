set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# 数学库
if(NOT TARGET glm)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(glm-release)
    set_target_properties(glm PROPERTIES FOLDER ${third_party_folder}/glm-release)
endif()

# openGL
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    # set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

# Log
if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

# stb 通用多媒体处理库
if(NOT TARGET stb)
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()


# if(NOT TARGET physX)
    # option(SPDLOG_BUILD_EXAMPLE "" OFF)
    # option(SPDLOG_INSTALL "" OFF)
    # add_subdirectory(physx-4.1)
# endif()
