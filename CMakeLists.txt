cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)


set(TB_PROJECT_NAME tournament-builder)
project(${TB_PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(TB_MAIN_VERSION 1)
set(TB_RELEASE_VERSION 0)
set(TB_PATCH_VERSION 0)
set(TB_VERSION ${TB_MAIN_VERSION}.${TB_RELEASE_VERSION}.${TB_PATCH_VERSION})

set(TB_PROJECT_SUMMARY [[\
Tournament builder is a simple desktop application that produces a set of teams, given a list of people (with their associated skill level) and a list of all the possible skill levels allowed.

It was originally made in the context of tournament organization, but its algorithm can be applied in any similar situation that requires to group a set number of weighted resources evenly.
]])

set(TB_DEV_NAME "io.github.alessandrodoradev")

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

set(TB_OFFICIAL_NAME "${TB_PROJECT_NAME}-${TB_VERSION}")
set(TB_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(TB_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(TB_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

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
set(TB_MAIN_TARGET_NAME ${PROJECT_NAME})
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
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set(ABSL_MSVC_STATIC_RUNTIME 
		ON CACHE BOOL 
		"Link ABSL static runtime libraries"
	)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL
		OFF CACHE BOOL 
		"Link GLFW static runtime libraries"
	)
endif()


if(TB_PLATFORM STREQUAL MACOSX)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -L/usr/local/opt/llvm/lib/unwind -lunwind")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++ -L/usr/local/opt/llvm/lib/unwind -lunwind")
	enable_language(OBJC)
	enable_language(OBJCXX)
endif()

add_subdirectory(${TB_LIBS_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${TB_CORELIB_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(src)
add_subdirectory(${TB_TEST_DIR})



##################################################################################################
##################################################################################################
##################################################################################################
##################################################################################################


set(TB_ICON_SIZES 16x16 32x32 48x48 64x64 128x128 256x256)

file(GLOB TB_ICON_DIR_RAW_FILES icons/*)
set(TB_ICON_FILE_NAMES "")
set(TB_ICON_SIZES "")
foreach(P ${TB_ICON_DIR_RAW_FILES})
	get_filename_component(FNAME ${P} NAME)
	list(APPEND TB_ICON_FILE_NAMES "${FNAME}")
endforeach()
list(FILTER TB_ICON_FILE_NAMES INCLUDE REGEX [[icon([0-9]+)x([0-9]+)\.png]])

foreach(TB_FILE_NAME ${TB_ICON_FILE_NAMES})
	string(REGEX MATCH [[([0-9]+)x([0-9]+)]] TB_SIZE_MATCH "${TB_FILE_NAME}")
	list(APPEND TB_ICON_SIZES "${TB_SIZE_MATCH}")
endforeach()



if(TB_PLATFORM STREQUAL LINUX) 
	set(TB_APPIMAGE_DIR "${CMAKE_INSTALL_PREFIX}/${TB_OFFICIAL_NAME}.AppDir")

	install(CODE "
		set(TB_APPIMAGE_DIR \"${TB_APPIMAGE_DIR}\")\n
		set(TB_MAIN_TARGET_NAME \"${TB_MAIN_TARGET_NAME}\")\n
		set(TB_OFFICIAL_NAME \"${TB_OFFICIAL_NAME}\")\n
		set(TB_PROJECT_NAME \"${TB_PROJECT_NAME}\")\n
		set(TB_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")\n
		set(TB_MAIN_DIR \"${CMAKE_CURRENT_SOURCE_DIR}\")\n
		set(TB_VERSION \"${TB_VERSION}\")\n
		set(TB_PROJECT_SUMMARY \"${TB_PROJECT_SUMMARY}\")\n
		set(TB_DEV_NAME \"${TB_DEV_NAME}\")
	")

	install(TARGETS ${TB_MAIN_TARGET_NAME}
		RUNTIME DESTINATION ${TB_APPIMAGE_DIR}/usr/bin/
		CONFIGURATIONS Release
	)
	install(CODE [[
		message(STATUS "Removing absolute paths from executable ${TB_APPIMAGE_DIR}/usr/bin/${TB_MAIN_TARGET_NAME}...")
		execute_process(
			COMMAND sed -i -e "s#/usr#././#g" "${TB_APPIMAGE_DIR}/usr/bin/${TB_MAIN_TARGET_NAME}"
		)
	]])
	foreach(SIZE ${TB_ICON_SIZES})
		install(FILES "icons/icon${SIZE}.png"
			DESTINATION "${TB_APPIMAGE_DIR}/usr/share/icons/hicolor/${SIZE}/apps/"
			RENAME icon.png
		)
	endforeach()
	install(FILES README.md LICENSE
		DESTINATION "${TB_APPIMAGE_DIR}/"
	)
	install(CODE [[
		message(STATUS "executing python3 \"${TB_MAIN_DIR}/LinuxPackagingScript.py\" \"${TB_OFFICIAL_NAME}\" \"${TB_PROJECT_NAME}\" \"${TB_MAIN_TARGET_NAME}\" \"${TB_APPIMAGE_DIR}\" \"${TB_INSTALL_PREFIX}\" \"${TB_VERSION}\", \"${TB_DEV_NAME}\" \"${TB_PROJECT_SUMMARY}\"")
		execute_process(
			COMMAND python3 "${TB_MAIN_DIR}/LinuxPackagingScript.py" "${TB_OFFICIAL_NAME}" "${TB_PROJECT_NAME}" "${TB_MAIN_TARGET_NAME}" "${TB_APPIMAGE_DIR}" "${TB_INSTALL_PREFIX}" "${TB_VERSION}" "${TB_DEV_NAME}" "${TB_PROJECT_SUMMARY}"
		)
	]])
	
endif()

if(TB_PLATFORM STREQUAL WINDOWS)
	install(TARGETS ${TB_MAIN_TARGET_NAME}
		RUNTIME DESTINATION bin
		CONFIGURATIONS Release
	)
	install(FILES README.md LICENSE
		DESTINATION .
	)
endif()



set(CPACK_PACKAGE_NAME "${TB_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${TB_MAIN_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${TB_RELEASE_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${TB_PATCH_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_EXECUTABLES "tournament-builder;Tournament builder")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out/packages")
set(CPACK_PACKAGE_VENDOR "${TB_DEV_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${TB_PROJECT_SUMMARY}")
set(CPACK_PACKAGE_ICON "icons/icon512x512.png")

include(CPack)