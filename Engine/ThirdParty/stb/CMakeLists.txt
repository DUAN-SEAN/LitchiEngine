
file(GLOB stb_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")

add_library(stb INTERFACE ${stb_sources})

target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)