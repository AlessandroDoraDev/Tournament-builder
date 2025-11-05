cmake_minimum_required(VERSION 3.16)

project(my_project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

# Enable testing for the project
enable_testing()

# Add Google Test
add_subdirectory(libs/gtest)

add_subdirectory(core-lib)
add_subdirectory(libs/abseil-cpp)


set(my_includes_dir src/include)
set(my_impl_dir src/implementation)

add_library(small-gui STATIC
libs/small-gui/microui.c
libs/small-gui/renderer.c)

add_executable(main 
${my_impl_dir}/main_noiaFattaBene.cpp)


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
${test_dir})


# Declare dependency on the absl::strings library
target_link_libraries(main tournament-build-core-lib small-gui absl::flat_hash_set absl::hash absl::btree)

# Add the tests directory
add_subdirectory(tests)