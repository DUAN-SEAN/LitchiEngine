set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# 数学库 只用于显示工程,不参与编译Link
if(NOT TARGET glm)
    option(GLM_BUILD_EXAMPLE "" OFF)
    option(GLM_INSTALL "" OFF)
    # add_subdirectory(glm-release)
    add_subdirectory(glm-0.9.9.8)
    # set_target_properties(glm PROPERTIES FOLDER ${third_party_folder}/glm-release)
    set_target_properties(glm PROPERTIES FOLDER ${third_party_folder}/glm-0.9.9.8)
endif()

# openGL
if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw-3.3.8)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw-3.3.8)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw-3.3.8)
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

# # openGL
# if(NOT TARGET freetype)
    # add_subdirectory(freetype)
    # set_target_properties(freetype PROPERTIES FOLDER ${third_party_folder}/freetype)
# endif()

# # assimp fbx解析库
# if(NOT TARGET assimp)
    # set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
# endif()


# 有报错,暂时不导入
# # stb 通用多媒体处理库
# if(NOT TARGET rttr)
    # # include(rttr.cmake)
	# # option(BUILD_STATIC "" ON)
	# # option(BUILD_INSTALLER "" ON)
	# add_subdirectory(rttr/rttr)
    # set_target_properties(rttr PROPERTIES FOLDER ${third_party_folder}/rttr/rttr)
# endif()

# if(NOT TARGET physX)
    # option(SPDLOG_BUILD_EXAMPLE "" OFF)
    # option(SPDLOG_INSTALL "" OFF)
    # add_subdirectory(physx-4.1)
# endif()
