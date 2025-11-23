cmake_minimum_required(VERSION 3.16)

project(my_project)

set(CMAKE_CXX_STANDARD 20)

# Process Abseil's CMake build system

set(my_includes_dir include)
set(my_impl_dir implementation)

add_library(tournament-build-core-lib STATIC
${my_impl_dir}/matricesModule.cpp
${my_impl_dir}/globals.cpp
${my_impl_dir}/Config.cpp
${my_impl_dir}/TournamentConfig.cpp
${my_impl_dir}/TeamArray.cpp
${my_impl_dir}/strat.cpp
${my_impl_dir}/to_strings.cpp
${my_impl_dir}/TournamentBuilderAPI.cpp
)


target_compile_definitions(tournament-build-core-lib 
PRIVATE 
$<$<CONFIG:Debug>:DEBUG>
$<$<CONFIG:Release>:NDEBUG>
#PROFILING_FLAG
)

if (MSVC)
add_link_options(
  $<$<CONFIG:Debug>:/INCREMENTAL> 
  $<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>
)
endif()

set_target_properties(tournament-build-core-lib PROPERTIES LINK_DEPENDS_NO_SHARED TRUE)

target_include_directories(tournament-build-core-lib PRIVATE 
${my_includes_dir}
libs
${test_dir})


# Declare dependency on the absl::strings library
target_link_libraries(tournament-build-core-lib absl::flat_hash_set absl::hash absl::btree)