cmake_minimum_required(VERSION 3.21)

project(LLVMSupport CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/build.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/functions.cmake)


set(PROJECT_SOURCE_FILES
	Allocator.cpp
	APFloat.cpp
	APInt.cpp
	APSInt.cpp
	ARMBuildAttrs.cpp
	ARMWinEH.cpp
	assert.cpp
	Atomic.cpp
	BlockFrequency.cpp
	BranchProbability.cpp
	circular_raw_ostream.cpp
	COM.cpp
	CommandLine.cpp
	Compression.cpp
	ConvertUTF.c
	ConvertUTFWrapper.cpp
	CrashRecoveryContext.cpp
	DAGDeltaAlgorithm.cpp
	DataExtractor.cpp
	DataStream.cpp
	Debug.cpp
	DeltaAlgorithm.cpp
	Dwarf.cpp
	DynamicLibrary.cpp
	Errno.cpp
	ErrorHandling.cpp
	FileOutputBuffer.cpp
	FileUtilities.cpp
	FoldingSet.cpp
	FormattedStream.cpp
	GraphWriter.cpp
	Hashing.cpp
	Host.cpp
	IntEqClasses.cpp
	IntervalMap.cpp
	IntrusiveRefCntPtr.cpp
	LEB128.cpp
	LineIterator.cpp
	Locale.cpp
	LockFileManager.cpp
	ManagedStatic.cpp
	MathExtras.cpp
	MD5.cpp
	Memory.cpp
	MemoryBuffer.cpp
	MemoryObject.cpp
	MSFileSystemBasic.cpp
	Mutex.cpp
	Options.cpp
	Path.cpp
	PrettyStackTrace.cpp
	Process.cpp
	Program.cpp
	RandomNumberGenerator.cpp
	raw_ostream.cpp
	raw_os_ostream.cpp
	regcclass.h
	regcname.h
	regcomp.c
	regengine.inc
	regerror.c
	Regex.cpp
	regex2.h
	regexec.c
	regex_impl.h
	regfree.c
	regmalloc.cpp
	regstrlcpy.c
	regutils.h
	RWMutex.cpp
	ScaledNumber.cpp
	SearchForAddressOfSpecialSymbol.cpp
	Signals.cpp
	SmallPtrSet.cpp
	SmallVector.cpp
	SourceMgr.cpp
	SpecialCaseList.cpp
	Statistic.cpp
	StreamingMemoryObject.cpp
	StringExtras.cpp
	StringMap.cpp
	StringPool.cpp
	StringRef.cpp
	StringSaver.cpp
	SystemUtils.cpp
	TargetParser.cpp
	TargetRegistry.cpp
	Threading.cpp
	ThreadLocal.cpp
	Timer.cpp
	TimeValue.cpp
	TimeProfiler.cpp
	ToolOutputFile.cpp
	Triple.cpp
	Twine.cpp
	Unicode.cpp
	Unix
	Valgrind.cpp
	Watchdog.cpp
	Windows
	YAMLParser.cpp
	YAMLTraits.cpp
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_FILES})

if(NOT MSVC)
	target_compile_definitions(${PROJECT_NAME} PRIVATE "__EMULATE_UUID=1")
endif()

target_include_directories(
	${PROJECT_NAME}
	BEFORE
	PRIVATE
	./
	../../../include/
	../../../include/dxc/Tracing/
	../../../include/llvm/llvm_assert/
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

set_target_cxx_standard(${PROJECT_NAME} 14)
set_target_c_standard(${PROJECT_NAME} 11)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/default_target_settings.cmake)
