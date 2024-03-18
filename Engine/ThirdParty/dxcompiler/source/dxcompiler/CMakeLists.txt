cmake_minimum_required(VERSION 3.21)

project(dxcompiler CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/build.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/functions.cmake)

file(GLOB_RECURSE PROJECT_SOURCE_FILES *.cpp)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_FILES})

set_target_cxx_standard(${PROJECT_NAME} 14)
set_target_c_standard(${PROJECT_NAME} 11)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(
	${PROJECT_NAME}
	BEFORE
	PRIVATE
	../../include/
	../../include/dxc/Tracing/
	../../include/llvm/llvm-assert/
)


target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_SPIRV_CODEGEN=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_DEPRECATE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_NONSTDC_NO_DEPRECATE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_NONSTDC_NO_WARNINGS)
target_compile_definitions(${PROJECT_NAME} PRIVATE _SCL_SECURE_NO_DEPRECATE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _SCL_SECURE_NO_WARNINGS)
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_CONSTANT_MACROS)
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_FORMAT_MACROS)
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_LIMIT_MACROS)
target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CINDEX_LIB_=)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBCLANG_CC=)

if (MSVC)
	set(LLVMDxilDia diaguids.lib)
	set(LLVMDxilDia LLVMDxilDia)
	target_compile_definitions(${PROJECT_NAME} PRIVATE "DXC_API_IMPORT=__declspec(dllexport)")
	target_link_options(${PROJECT_NAME} PRIVATE /LIBPATH:"$(VsInstallDir)/DIA SDK/lib/amd64")
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE "__EMULATE_UUID=1")
endif()

target_link_directories(${PROJECT_NAME} PRIVATE ${TARGET_TMP_DIR}/lib/${PROJECT_OUTPUT_SUB_PATH})

set(DXC_LIBRARIES
	clangIndex
	clangRewrite
	clangCodeGen
	clangFrontendTool
	clangFrontend
	clangDriver
	clangSema
	clangEdit
	clangAST
	clangCodeGen
	clangLex
	clangTooling
	clangBasic
	clangFormat
	clangParse
	clangSPIRV
	libclang
	LLVMAnalysis
	LLVMAsmParser
	LLVMBitcode
	LLVMCore
	LLVMDxcBindingTable
	LLVMDxcSupport
	LLVMDXIL
	LLVMDxilCompression
	LLVMDxilContainer
	${LLVMDxilDia}
	LLVMDxilPdbInfo
	LLVMDxilPIXPasses
	LLVMDxilRootSignature
	LLVMDxrFallback
	LLVMHLSL
	LLVMInstCombine
	LLVMMIPO
	LLVMIRReader
	LLVMLinker
	LLVMLTO
	LLVMMSSupport
	LLVMOption
	LLVMPassPrinters
	LLVMProfileData
	LLVMScalarOpts
	LLVMSupport
	LLVMTableGen
	LLVMTarget
	LLVMTransformUtils
	LLVMVectorize
	clangAnalysis
	clangASTMatchers
	SPIRV-Tools
	SPIRV-Tools-opt
	${DXC_DIAG_GUIDS}
)
target_link_libraries(${PROJECT_NAME}
	${DXC_LIBRARIES}
	${DXC_LIBRARIES}
)


include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/default_target_settings.cmake)

set(DXCOMPILER_HEADERS ../../include/dxc/dxcapi.h ../../include/dxc/config.h  ../../include/dxc/dxcerrors.h ../../include/dxc/dxcisense.h ../../include/dxc/WinAdapter.h ../../include/dxc/Support/WinIncludes.h)

install(
    TARGETS dxcompiler
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
)

install(
    FILES ${DXCOMPILER_HEADERS}
    DESTINATION include
)