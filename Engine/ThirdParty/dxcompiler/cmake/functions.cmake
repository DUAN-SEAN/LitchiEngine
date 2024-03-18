include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckCSourceCompiles)
if(NOT PROJECT_DISABLE_SCAN_FOR_HEADERS)
include(FindThreads)
endif()

function(check_include_file_exits_or_fail FILE)
	check_include_file(${FILE} INCLUDE_EXIST_${FILE})
	if(NOT INCLUDE_EXIST_${FILE} EQUAL 1)
		message(FATAL_ERROR "Required header does exist, CMake will exit." )
	endif()
endfunction()

function(check_include_file_cxx_exits_or_fail FILE)
	check_include_file_cxx(${FILE} INCLUDE_EXIST_${FILE})
	if(NOT INCLUDE_EXIST_${FILE} EQUAL 1)
		message(FATAL_ERROR "Required header does exist, CMake will exit." )
	endif()
endfunction()

function(check_library_exits_or_fail LIBRARY FUNCTION)
	check_library_exists(${LIBRARY} ${FUNCTION} "" LIBRARAY_EXIST_${LIBRARY})
	if(NOT LIBRARAY_EXIST_${LIBRARY} EQUAL 1)
		message(FATAL_ERROR "Required library does exist, CMake will exit." )
	endif()
endfunction()

function(check_cxx_compiler_flag_or_fail FLAG)
    if(MSVC)
        check_cxx_compiler_flag("/${FLAG}" FLAG_SUPPORTED_${LIBRARY})
    else()
	    check_cxx_compiler_flag("-${FLAG}" FLAG_SUPPORTED_${LIBRARY})
    endif()
	if(NOT FLAG_SUPPORTED_${FLAG} EQUAL 1)
		message(FATAL_ERROR "Required feature is not supported, CMake will exit." )
	endif()
endfunction()

function(set_target_cxx_standard TARGET STANDARD)
    set_target_properties(${TARGET} PROPERTIES CXX_STANDARD ${STANDARD})
    set_target_properties(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED on)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS off)
endfunction()

function(set_target_c_standard TARGET STANDARD)
    set_target_properties(${TARGET} PROPERTIES C_STANDARD ${STANDARD})
    set_target_properties(${TARGET} PROPERTIES C_STANDARD_REQUIRED on)
    set_target_properties(${TARGET} PROPERTIES C_EXTENSIONS off)
endfunction()

function(add_custom_target_copy_header TARGET HEADER OUT_PATH)
    set(ABS_PATH_HEADER ${HEADER})
    set(ABS_PATH_OUT ${OUT_PATH})
    cmake_path(ABSOLUTE_PATH ABS_PATH_HEADER NORMALIZE)
    cmake_path(ABSOLUTE_PATH OUT_PATH NORMALIZE)

    get_filename_component(FILE_NAME ${ABS_PATH_HEADER} NAME)
	add_custom_target(${TARGET} ALL DEPENDS ${ABS_PATH_OUT}/${FILE_NAME})
    add_custom_command(
        TARGET
            ${TARGET}
            POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy 
            ${ABS_PATH_HEADER}
            ${ABS_PATH_OUT}/${FILE_NAME}
        COMMENT
            "Copy ${HEADER} to ${ABS_PATH_OUT}"
    )
endfunction()

