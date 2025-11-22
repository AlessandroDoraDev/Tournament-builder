  cmake_minimum_required(VERSION 3.20)

project(my_project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

include(CTest)
enable_testing()

add_subdirectory(libs/gtest)
add_subdirectory(libs/nativefiledialog-extended)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)

add_subdirectory(libs/GLFW)

add_subdirectory(core-lib)
add_subdirectory(libs/abseil-cpp)

set(my_includes_dir src/include)
set(my_impl_dir src/implementation)

# link_directories(
#   libs/GLFW/build/src/Debug
#   out/build/Linux_ConfigPreset/libs/GLFW/src/
# )


add_library(imgui STATIC
libs/imgui/imgui.cpp
libs/imgui/imgui_demo.cpp
libs/imgui/imgui_draw.cpp
libs/imgui/imgui_tables.cpp
libs/imgui/imgui_widgets.cpp
libs/imgui/backends/imgui_impl_glfw.cpp
libs/imgui/backends/imgui_impl_opengl3.cpp
libs/imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(stb STATIC
  libs/stb/stb_image.cpp
)

target_include_directories(imgui
PRIVATE
libs/imgui
libs/imgui/backends
libs/imgui/misc
libs/GLFW/include
)

target_include_directories(stb
PRIVATE
libs/stb
)

add_executable(main WIN32
${my_impl_dir}/main_noiaFattaBene.cpp
${my_impl_dir}/TournamentBuilderGui.cpp
${my_impl_dir}/TournamentBuilderAPI.cpp
${my_impl_dir}/WindowsDefs.cpp
tests/utils.cpp)

if (MSVC)
  target_link_options(main PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
endif()

target_compile_definitions(main 
PRIVATE 
$<$<CONFIG:Debug>:MIO_DEBUG>
$<$<CONFIG:Release>:MIO_RELEASE>
#PROFILING_FLAG
)

if (MSVC)
add_link_options(
  $<$<CONFIG:Debug>:/INCREMENTAL> 
  $<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>
)
endif()

set_target_properties(main PROPERTIES LINK_DEPENDS_NO_SHARED TRUE)

target_include_directories(main 
PRIVATE 
${my_includes_dir}
libs
core-lib/include
libs/imgui
libs/imgui/backends
libs/imgui/misc/cpp
libs/GLFW/include
libs/
libs/nativefiledialog-extended/src/include
libs/stb
tests/
${test_dir})



target_link_libraries(main 
tournament-build-core-lib 
imgui
glfw
stb
nfd
absl::flat_hash_set 
absl::hash absl::btree
absl::fixed_array)

if(WIN32)
target_link_libraries(main
ole32
uuid
shell32
)
endif()

add_custom_command(
  TARGET main
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/icons"
    "$<TARGET_FILE_DIR:main>/icons"
)

target_link_libraries(imgui
glfw
)

set(test_dir tests)

add_executable(test_target
    ${test_dir}/main_test.cpp
    ${test_dir}/utils.cpp
)
set_target_properties(main PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
set_target_properties(test_target PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
set_target_properties(tournament-build-core-lib PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
target_include_directories(test_target
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/gtest/googletest/include
    ${CMAKE_SOURCE_DIR}/libs/gtest/googlemock/include
    ${CMAKE_SOURCE_DIR}/${my_includes_dir}
    ${test_dir}
    ${CMAKE_SOURCE_DIR}/core-lib/include
)

link_directories(test_target
    ${CMAKE_CURRENT_BINARY_DIR}/core-lib/Release
)

target_link_libraries(test_target 
    PRIVATE
    GTest::gtest_main
    absl::flat_hash_set 
    absl::hash 
    absl::btree
    tournament-build-core-lib
)

include(GoogleTest)
gtest_discover_tests(test_target)