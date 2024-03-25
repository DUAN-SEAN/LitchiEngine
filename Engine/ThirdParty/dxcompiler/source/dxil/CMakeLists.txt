cmake_minimum_required(VERSION 3.21)

project(dxil VERSION 1.0 DESCRIPTION "dxil")

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/build.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/functions.cmake)

if(MSVC)
	install(
		FILES ${CMAKE_CURRENT_LIST_DIR}/windows/x64/dxil.dll
		DESTINATION bin
	)
endif()
