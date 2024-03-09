cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# FLAGS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPHYSX_PROFILE_SDK -DRENDER_SNIPPET -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPHYSX_PROFILE_SDK -DRENDER_SNIPPET -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -g3 -gdwarf-2 -O0")

    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
elseif(WIN32)
    # TODO: Set PX_PHYSX_CORE_EXPORTS flag and enable delay loads.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t /D \"_HAS_EXCEPTIONS=0\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PHYSX_PROFILE_SDK\" /D \"RENDER_SNIPPET\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /GF /Zc:forScope /GR- /Gd /MT /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+ /Zc:inline /fp:fast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t /D \"_HAS_EXCEPTIONS=0\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PHYSX_PROFILE_SDK\" /D \"RENDER_SNIPPET\" /D \"_DEBUG\" /D \"PX_DEBUG=1\" /D \"PX_CHECKED=1\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /GF /Zc:forScope /RTCu /GR- /Gd /MTd /Zc:inline /fp:fast /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPHYSX_PROFILE_SDK -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPHYSX_PROFILE_SDK -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -g3 -O0")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# FILES
# -----------------------------------------------------------------------------------------------------------------------------

# Headers
file(GLOB_RECURSE SNIPPET_HELLO_WORLD_HEADERS ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Snippets/SnippetHelloWorld/*.h)

# Sources
file(GLOB_RECURSE SNIPPET_HELLO_WORLD_SOURCES ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Snippets/SnippetHelloWorld/*.cpp)
list(APPEND SNIPPET_HELLO_WORLD_SOURCES ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Snippets/SnippetCommon/ClassicMain.cpp)

# Source groups
source_group("Headers" FILES ${SNIPPET_HELLO_WORLD_HEADERS})
source_group("Sources" FILES ${SNIPPET_HELLO_WORLD_SOURCES})

# -----------------------------------------------------------------------------------------------------------------------------
# TARGETS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    add_executable(SnippetHelloWorld MACOSX_BUNDLE ${SNIPPET_HELLO_WORLD_HEADERS} ${SNIPPET_HELLO_WORLD_SOURCES})
else()
    add_executable(SnippetHelloWorld ${SNIPPET_HELLO_WORLD_HEADERS} ${SNIPPET_HELLO_WORLD_SOURCES})
endif()

set_target_properties(SnippetHelloWorld PROPERTIES FOLDER "Snippets")

target_include_directories(SnippetHelloWorld PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include")
target_include_directories(SnippetHelloWorld PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/include")
target_include_directories(SnippetHelloWorld PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/foundation/include")
target_include_directories(SnippetHelloWorld PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/fastxml/include")

if(WIN32)
    target_include_directories(SnippetHelloWorld PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Snippets/Graphics/include/win32/GL")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# LINKING
# -----------------------------------------------------------------------------------------------------------------------------

# Link PhysX3 libs
if(WIN32 OR UNIX OR APPLE)
    target_link_libraries(SnippetHelloWorld PhysX3Common)
    target_link_libraries(SnippetHelloWorld PhysX3)
    target_link_libraries(SnippetHelloWorld PhysX3Vehicle)
    target_link_libraries(SnippetHelloWorld PhysX3Cooking)
    target_link_libraries(SnippetHelloWorld PhysX3Extensions)
    target_link_libraries(SnippetHelloWorld PhysX3CharacterKinematic)
    target_link_libraries(SnippetHelloWorld PxFoundation)
    target_link_libraries(SnippetHelloWorld PsFastXml)
    target_link_libraries(SnippetHelloWorld PxPvdSDK)
    target_link_libraries(SnippetHelloWorld SimulationController)
    target_link_libraries(SnippetHelloWorld SceneQuery)
    target_link_libraries(SnippetHelloWorld LowLevel)
    target_link_libraries(SnippetHelloWorld LowLevelAABB)
    target_link_libraries(SnippetHelloWorld LowLevelDynamics)
    target_link_libraries(SnippetHelloWorld LowLevelCloth)
    target_link_libraries(SnippetHelloWorld LowLevelParticles)
    target_link_libraries(SnippetHelloWorld PxTask)

    # Link Snippet libs
    target_link_libraries(SnippetHelloWorld SnippetUtils)
    target_link_libraries(SnippetHelloWorld SnippetRender)
endif()

if(WIN32)
    target_link_libraries(SnippetHelloWorld "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Snippets/Graphics/lib/win64/glut/glut32.lib")
endif()

# Link OS libs
if(APPLE)
    # Link Apple Frameworks
    target_link_libraries(SnippetHelloWorld "-framework OpenGL"
                                            "-framework GLUT")

    set(MACOSX_BUNDLE_BUNDLE_NAME "SnippetHelloWorld")
    set(MAC_HIGH_RESOLUTION_CAPABLE "true")
endif() 

if(EMSCRIPTEN)
    set(LINK_LIBS PhysX3Common PhysX3 PhysX3Vehicle PhysX3Cooking PhysX3Extensions PhysX3CharacterKinematic PxFoundation PsFastXml PxPvdSDK SimulationController SceneQuery LowLevel LowLevelAABB LowLevelDynamics LowLevelCloth LowLevelParticles PxTask)
    target_link_libraries(SnippetHelloWorld ${LINK_LIBS} ${LINK_LIBS})
    set_target_properties(SnippetHelloWorld PROPERTIES LINK_FLAGS "-s LEGACY_GL_EMULATION=1 -o main.html --std=c++11")
endif()