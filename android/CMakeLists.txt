# Android build wrapper for MapleStory-Client.
#
# This is a SCAFFOLD. To actually run on Android you must first port the renderer
# to OpenGL ES (see Platform/NOTES.md, section 2.1) and add touch input. Until
# then this will configure but the resulting libmain.so will not render.
#
# Prerequisites:
#   * Android NDK (set ANDROID_NDK environment variable)
#   * SDL3 built for Android, or the SDL3 AAR, providing the `SDL3::SDL3` target
#   * OpenGL ES / EGL available in the NDK sysroot (they are)
#
# Build:
#   cmake -S android -B android/build \
#     -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%/build/cmake/android.toolchain.cmake \
#     -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
#     -DMOBILE_OPENGL_ES=ON
#   cmake --build android/build
#
# The parent CMakeLists.txt detects ANDROID and produces a shared library named
# "main" (libmain.so), which SDL's Android activity loads and which calls our
# main(). It links SDL3 + GLESv2 + EGL + log + android.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ENV{ANDROID_NDK} AND NOT CMAKE_TOOLCHAIN_FILE)
	message(FATAL_ERROR "Set ANDROID_NDK or pass -DCMAKE_TOOLCHAIN_FILE=<ndk>/build/cmake/android.toolchain.cmake")
endif()

if(NOT CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake"
		CACHE FILEPATH "Android NDK toolchain")
endif()

set(ANDROID_ABI ${ANDROID_ABI} CACHE STRING "ABI" FORCE)
set(ANDROID_PLATFORM ${ANDROID_PLATFORM} CACHE STRING "API level" FORCE)
set(MOBILE_OPENGL_ES ON CACHE BOOL "Use OpenGL ES renderer" FORCE)

# Reuse the top-level build as a subdirectory (it creates the `main` shared lib).
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/client)
