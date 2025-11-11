cmake_minimum_required(VERSION 3.16)

project(my_project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include(CTest)
enable_testing()

add_subdirectory(libs/gtest)

add_subdirectory(core-lib)
add_subdirectory(libs/abseil-cpp)

set(my_includes_dir src/include)
set(my_impl_dir src/implementation)

link_directories(
  libs/GLFW/build/src/Debug
)


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

add_executable(main 
${my_impl_dir}/main_noiaFattaBene.cpp
${my_impl_dir}/TournamentBuilderGui.cpp
${my_impl_dir}/TournamentBuilderAPI.cpp
tests/utils.cpp)


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
libs/stb
tests/
${test_dir})



target_link_libraries(main 
tournament-build-core-lib 
imgui
glfw3
stb
absl::flat_hash_set 
absl::hash absl::btree)

target_link_libraries(imgui
glfw3
)

set(test_dir tests)

add_executable(test_target
    ${test_dir}/main_test.cpp
    ${test_dir}/utils.cpp
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
    ../out/build/MSVC_ConfigPreset/core-lib/Release
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