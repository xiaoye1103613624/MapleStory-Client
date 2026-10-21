# =============================================================================
# MapleStory-Client — cross-platform build
# -----------------------------------------------------------------------------
# Targets:
#   * Windows  (MSVC / MinGW)  — uses the bundled prebuilt libs in includes/
#   * Linux / macOS            — dependencies resolved via find_package / vcpkg
#   * Android / iOS            — scaffolded under android/ and ios/ (see notes)
#
# The renderer is currently desktop OpenGL (GLEW). To actually *run* on phones
# it must be ported to OpenGL ES and the input layer to touch — that is a
# separate phase (documented in docs/features/cross-platform-build/README.md).
# This CMake file makes the *code* build on every listed platform; the mobile
# renderer/input/audio porting is follow-up work.
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(MapleStoryClient CXX)

# Architecture folder names used by the bundled Windows libs.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCH x64)
	set(ARCHW winx64)
	set(LZ4_ARCH win64)
else()
	set(ARCH Win32)
	set(ARCHW winx86)
	set(LZ4_ARCH win32)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- Options ----------------------------------------------------------------
option(USE_BASS "Use the BASS audio backend (needs BASS libs)" ON)
option(BUILD_MOBILE "Configure for a mobile (Android/iOS) target" OFF)
option(MOBILE_OPENGL_ES "Use an OpenGL ES renderer (mobile only; not yet ported)" OFF)

# The POSIX socket backend is auto-selected on non-Windows by MapleStory.h.
# Windows keeps Winsock; the Asio path exists behind USE_ASIO.

# ---- Source set -------------------------------------------------------------
set(SRC_DIRS Audio Character Data Gameplay Graphics IO Net Util)
set(CLIENT_SOURCES)
foreach(dir ${SRC_DIRS})
	file(GLOB_RECURSE TMP "${CMAKE_SOURCE_DIR}/${dir}/*.cpp")
	list(APPEND CLIENT_SOURCES ${TMP})
endforeach()
list(APPEND CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/MapleStory.cpp")
list(APPEND CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/Configuration.cpp")
# NoLifeNx sources.
# On Windows/Linux/Apple a prebuilt nlnx library is linked separately (the
# #pragma comment(lib) in NxFiles.h on Windows, or find_library elsewhere),
# so only the nx.cpp loader is compiled here; the rest comes from that lib.
# On Android there is no prebuilt, so the whole nlnx sourceset is compiled
# directly into the target (it only depends on zlib/lz4/squish, all provided).
if(ANDROID)
	list(APPEND CLIENT_SOURCES
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/audio.cpp"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/bitmap.cpp"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/file.cpp"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/node.cpp"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/nx.cpp"
	)
else()
	list(APPEND CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/nx.cpp")
endif()
# Vendor key table (the rest of NoLifeWzToNx is embedded via Util/WzRuntime.cpp).
list(APPEND CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/tools/vendor/NoLifeWzToNx-master/Keys.cpp")

if(WIN32)
	list(APPEND CLIENT_SOURCES "${CMAKE_SOURCE_DIR}/MapleStory.rc")
endif()

# ---- Common include directories --------------------------------------------
set(INCLUDE_DIRS
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/includes/glew-2.1.0/include/GL"
	"${CMAKE_SOURCE_DIR}/includes/freetype/include"
	"${CMAKE_SOURCE_DIR}/includes/SDL3/include"
	"${CMAKE_SOURCE_DIR}/includes/stb"
	"${CMAKE_SOURCE_DIR}/includes/bass24/c"
	"${CMAKE_SOURCE_DIR}/includes/NoLifeNx"
	"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/includes/lz4_v1_8_2_${LZ4_ARCH}/include"
	"${CMAKE_SOURCE_DIR}/tools/vendor/NoLifeWzToNx-master/includes/zlib/include"
	"${CMAKE_SOURCE_DIR}/tools/vendor/NoLifeWzToNx-master/includes/libsquish"
)

if(ANDROID)
	# SDL on Android loads a shared library named "main" that calls our main().
	set(TARGET_NAME main)
	add_library(${TARGET_NAME} SHARED ${CLIENT_SOURCES})
else()
	set(TARGET_NAME MapleStory)
	add_executable(${TARGET_NAME} ${CLIENT_SOURCES})
endif()
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRS})

# ---- Compile definitions ----------------------------------------------------
# The original MSVC project force-includes these three headers (ForcedIncludeFiles).
# Several translation units rely on them being present, so replicate that for every
# compiler to keep the source set unchanged and portable.
target_compile_options(${TARGET_NAME} PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/FIstring /FIcmath /FIstdexcept>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-include string>
)

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PRIVATE USE_BASS _WIN32)
	target_compile_options(${TARGET_NAME} PRIVATE
		# MSVC：/utf-8 同时固定"源字符集"与"执行字符集"，源码按 UTF-8 解释、
		# 字符串字面量按 UTF-8 产出（与 MapleStory.vcxproj 的 /utf-8 一致）。
		$<$<CXX_COMPILER_ID:MSVC>:/utf-8 /bigobj>
		# MinGW 等非 MSVC 的 Windows 编译器：显式固定输入/执行字符集，
		# 保证与 MSVC 语义一致，避免不同编译器对同一份 UTF-8 中文注释/字面量
		# 解释出不同结果。
		$<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CXX_COMPILER_ID:GNU>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
		$<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CXX_COMPILER_ID:Clang>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
	)
	# Match MSVC project: Release is a GUI (no console) executable.
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
else()
	# Non-Windows: POSIX socket backend is auto-enabled by MapleStory.h.
	# BASS is OFF unless the user provides the libs.
	if(NOT USE_BASS)
		target_compile_definitions(${TARGET_NAME} PRIVATE USE_BASS=0)
	endif()

	# 与 Windows 分支对齐：显式固定输入/执行字符集，确保源码中的 UTF-8
	# 中文注释与字符串字面量在任何编译器下解释一致。
	target_compile_options(${TARGET_NAME} PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
		$<$<CXX_COMPILER_ID:Clang>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
	)
endif()

if(MOBILE_OPENGL_ES)
	target_compile_definitions(${TARGET_NAME} PRIVATE USE_OPENGL_ES)
endif()

# ---- Per-platform linkage ---------------------------------------------------
if(WIN32)
	set(LIB_DIRS
		"${CMAKE_SOURCE_DIR}/includes/glew-2.1.0/lib/Release/${ARCH}"
		"${CMAKE_SOURCE_DIR}/includes/freetype/${LZ4_ARCH}"
		"${CMAKE_SOURCE_DIR}/includes/SDL3/lib/${ARCH}"
		"${CMAKE_SOURCE_DIR}/includes/bass24/c/${ARCH}"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/${ARCH}/$<CONFIG>"
		"${CMAKE_SOURCE_DIR}/tools/vendor/NoLifeWzToNx-master/includes/zlib/lib"
		"${CMAKE_SOURCE_DIR}/tools/vendor/NoLifeWzToNx-master/includes/libsquish/${LZ4_ARCH}/$<CONFIG>"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/includes/lz4_v1_8_2_${LZ4_ARCH}/dll"
	)
	target_link_directories(${TARGET_NAME} PRIVATE ${LIB_DIRS})

	target_link_libraries(${TARGET_NAME} PRIVATE
		glew32s freetype SDL3 bass OpenGL32 Iphlpapi
		zlib libsquish liblz4
		ws2_32
	)

	# Copy required DLLs next to the executable (only those present in the tree).
	# bass.dll is NOT bundled here — it must be supplied at runtime (the deploy
	# package in E:\MXD\BeiDou-Client-xy already provides it).
	set(DLL_SRCS
		"${CMAKE_SOURCE_DIR}/includes/freetype/${LZ4_ARCH}/freetype.dll"
		"${CMAKE_SOURCE_DIR}/includes/bass24/c/${ARCH}/bass.dll"
		"${CMAKE_SOURCE_DIR}/includes/NoLifeNx/nlnx/includes/lz4_v1_8_2_${LZ4_ARCH}/dll/liblz4.so.1.8.2.dll"
	)
	foreach(dll ${DLL_SRCS})
		if(EXISTS "${dll}")
			add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
				VERBATIM)
		endif()
	endforeach()

elseif(APPLE)
	find_package(SDL3 REQUIRED)
	find_package(Freetype REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE
		SDL3::SDL3
		Freetype::Freetype
		${OPENGL_LIBRARIES}
		GLEW::GLEW
	)
	# On macOS, BASS is optional; provide it via BASS_ROOT if available.
	if(USE_BASS)
		find_library(BASS_LIB NAMES bass HINTS "${BASS_ROOT}" "$ENV{BASS_ROOT}")
		if(BASS_LIB)
			target_link_libraries(${TARGET_NAME} PRIVATE ${BASS_LIB})
		else()
			message(WARNING "USE_BASS=ON but libbass not found; audio will be silent.")
		endif()
	endif()
	# NoLifeNx/zlib/libsquish/lz4 must be provided (e.g. via vcpkg / Homebrew / ports).
	find_library(NOLIFENX_LIB NAMES nlnx HINTS "${NOLIFENX_ROOT}")
	find_library(ZLIB_LIB NAMES z)
	find_library(SQUISH_LIB NAMES squish)
	find_library(LZ4_LIB NAMES lz4)
	foreach(lib NOLIFENX_LIB ZLIB_LIB SQUISH_LIB LZ4_LIB)
		if(${lib})
			target_link_libraries(${TARGET_NAME} PRIVATE ${${lib}})
		endif()
	endforeach()

elseif(ANDROID)
	# Prebuilt dependency tree produced by tools/android/build_android_deps.ps1
	# (SDL3 / freetype / lz4 / squish static libs under E:/Android/deps/<lib>/<abi>).
	# The emulator image we target is x86_64, so the ABI matches ANDROID_ABI.
	set(ANDROID_DEPS_ROOT "E:/Android/deps" CACHE PATH "prebuilt android deps root")
	set(DEP_ABI "${ANDROID_ABI}")

	# lz4 headers: use the exact source we compiled (v1.10.0) so nx.cpp's LZ4
	# calls match the linked liblz4.a. BEFORE so it wins over the bundled v1.8.2.
	target_include_directories(${TARGET_NAME} BEFORE PRIVATE
		"${ANDROID_DEPS_ROOT}/sdl3/${DEP_ABI}/include"
		"${ANDROID_DEPS_ROOT}/freetype/${DEP_ABI}/include/freetype2"
		"E:/Android/src/lz4-1.10.0/lib"
	)
	# squish public headers come from the repo vendor tree (already in INCLUDE_DIRS).

	# GLES renderer on. USE_BASS is intentionally left undefined so Audio.cpp
	# compiles its silent fallback (no BASS libs are bundled for Android).
	target_compile_definitions(${TARGET_NAME} PRIVATE USE_OPENGL_ES)

	target_link_directories(${TARGET_NAME} PRIVATE
		"${ANDROID_DEPS_ROOT}/sdl3/${DEP_ABI}/lib"
		"${ANDROID_DEPS_ROOT}/freetype/${DEP_ABI}/lib"
		"${ANDROID_DEPS_ROOT}/lz4/${DEP_ABI}/lib"
		"${ANDROID_DEPS_ROOT}/squish/${DEP_ABI}/lib"
	)
	target_link_libraries(${TARGET_NAME} PRIVATE
		SDL3 freetype lz4 squish
		EGL GLESv2 log android z OpenSLES
	)

else()
	# Linux / MinGW (non-Windows, non-Apple, non-Android)
	find_package(SDL3 REQUIRED)
	find_package(Freetype REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE
		SDL3::SDL3
		Freetype::Freetype
		${OPENGL_LIBRARIES}
		GLEW::GLEW
		pthread
		dl
	)
	if(USE_BASS)
		find_library(BASS_LIB NAMES bass HINTS "${BASS_ROOT}" "$ENV{BASS_ROOT}")
		if(BASS_LIB)
			target_link_libraries(${TARGET_NAME} PRIVATE ${BASS_LIB})
		else()
			message(WARNING "USE_BASS=ON but libbass not found; audio will be silent.")
		endif()
	endif()
	find_library(NOLIFENX_LIB NAMES nlnx HINTS "${NOLIFENX_ROOT}")
	find_library(ZLIB_LIB NAMES z)
	find_library(SQUISH_LIB NAMES squish)
	find_library(LZ4_LIB NAMES lz4)
	foreach(lib NOLIFENX_LIB ZLIB_LIB SQUISH_LIB LZ4_LIB)
		if(${lib})
			target_link_libraries(${TARGET_NAME} PRIVATE ${${lib}})
		endif()
	endforeach()
endif()

# ---- Output directory (preserve the existing deploy workflow if requested) ---
if(DEFINED ENV{MAPLESTORY_DEPLOY_DIR})
	set_target_properties(${TARGET_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "$ENV{MAPLESTORY_DEPLOY_DIR}")
endif()
