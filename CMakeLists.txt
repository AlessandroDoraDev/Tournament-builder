cmake_minimum_required(VERSION 3.16)

project(my_project)

set(CMAKE_CXX_STANDARD 20)

# Enable testing for the project
enable_testing()

# Process Abseil's CMake build system
add_subdirectory(libs/abseil-cpp)

# Add Google Test
add_subdirectory(libs/gtest)


set(my_includes_dir src/includes)
set(my_impl_dir src/implementation)

add_executable(main 
${my_impl_dir}/main_noiaFattaBene.cpp
${my_impl_dir}/matricesModule.cpp
${my_impl_dir}/globals.cpp
${my_impl_dir}/Config.cpp
${my_impl_dir}/TournamentConfig.cpp
${my_impl_dir}/TeamArray.cpp
${my_impl_dir}/strat.cpp
${my_impl_dir}/to_strings.cpp)


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
${test_dir})


# Declare dependency on the absl::strings library
target_link_libraries(main absl::flat_hash_set absl::hash absl::btree)

# Add the tests directory
add_subdirectory(tests)