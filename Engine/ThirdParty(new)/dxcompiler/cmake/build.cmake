
include(${CMAKE_CURRENT_LIST_DIR}/functions.cmake)


set(CMAKE_USE_RELATIVE_PATHS off)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_CONFIGURATION_TYPES ${TARGET_CONFIG})
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(CMAKE_IMPORT_LIBRARY_PREFIX "lib")

if(${TARGET_BUILD_PLATFORM} STREQUAL "linux")
	find_program(CCACHE_PROGRAM ccache)
	if(CCACHE_PROGRAM)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
	endif()
endif()


if(NOT PROJECT_DISABLE_SCAN_FOR_HEADERS)
	check_include_file_exits_or_fail(stdint.h)
	check_include_file_exits_or_fail(stdio.h)
	check_include_file_exits_or_fail(wchar.h)
	check_include_file_exits_or_fail(stdlib.h)
	check_include_file_exits_or_fail(stddef.h)
	check_include_file_exits_or_fail(time.h)
	check_include_file_exits_or_fail(math.h)
	check_include_file_exits_or_fail(float.h)
	check_include_file_exits_or_fail(ctype.h)
	check_include_file_exits_or_fail(sys/stat.h)
	check_include_file_exits_or_fail(wctype.h)
	check_include_file_exits_or_fail(locale.h)
	check_include_file_exits_or_fail(errno.h)
	check_include_file_exits_or_fail(string.h)

	if(NOT MSVC)
		check_include_file_cxx_exits_or_fail(cxxabi.h)
	endif()

endif()
