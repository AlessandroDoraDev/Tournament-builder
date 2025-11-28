cmake_minimum_required(VERSION 3.20)

project(tournament-builder LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

set(TB_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(TB_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(TB_ASSETS_DIR ${CMAKE_CURRET_SOURCE_DIR}/assets)

#include dirs vars
set(TB_NFDE_INCLUDE_DIRS
	${TB_LIBS_DIR}/nativefiledialog-extended/src/include
)
set(TB_GLFW_INCLUDE_DIRS 
	${TB_LIBS_DIR}/libs/GLFW/include
)
set(TB_IMGUI_INCLUDE_DIRS
	${TB_LIBS_DIR}/imgui
	${TB_LIBS_DIR}/imgui/backends
	${TB_LIBS_DIR}/imgui/misc/cpp
)
set(TB_STB_INCLUDE_DIRS
	${TB_LIBS_DIR}/stb
)
set(TB_LIBS_INCLUDE_DIRS 
	${TB_NFDE_INCLUDE_DIRS}
	${TB_IMGUI_INCLUDE_DIRS}
	${TB_GLFW_INCLUDE_DIRS}
	${TB_STB_INCLUDE_DIRS}
)
set(TB_CORELIB_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/core-lib
)
set(TB_CORELIB_INCLUDE_DIRS
	${TB_CORELIB_DIR}/include
)
set(TB_GTEST_INCLUDE_DIRS
	${TB_LIBS_DIR}gtest/googletest/include
	${TB_LIBS_DIR}/gtest/googlemock/include
	${TB_CORELIB_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

set(TB_CORELIB_TARGET_NAME tournament-build-core-lib)
set(TB_MAIN_TARGET_NAME main)
set(TB_IMGUI_TARGET_NAME imgui)
set(TB_STB_TARGET_NAME stb)
set(TB_TESTS_TARGET_NAME tb_tests)


#setting vars for platforms identification and compiler identification (in case they ever change and because it's easier to remember)
set(TB_PLATFORM Undefined)
if(WIN32)
	set(TB_PLATFORM WINDOWS)
elseif(APPLE)
	set(TB_PLATFORM MACOSX)
elseif(UNIX AND NOT APPLE)
	set(TB_PLATFORM LINUX)
endif()

set(TB_CURRENT_COMPILER Undefined)
if(MSVC)
	set(TB_CURRENT_COMPILER COMPILER_MSVC)
endif()


if(${TB_PLATFORM} STREQUAL MACOSX)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -L/usr/local/opt/llvm/lib/unwind -lunwind")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -L/usr/local/opt/llvm/lib/unwind -lunwind")
	enable_language(OBJC)
	enable_language(OBJCXX)
endif()

add_subdirectory(${TB_LIBS_DIR})
add_subdirectory(${TB_CORELIB_DIR})
add_subdirectory(src)
add_subdirectory(${TB_TEST_DIR})
