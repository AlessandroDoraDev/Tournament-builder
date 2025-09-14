cmake_minimum_required(VERSION 3.16)

project(my_project)

set(CMAKE_CXX_STANDARD 20)

# Process Abseil's CMake build system
add_subdirectory(libs/abseil-cpp)

set(my_includes_dir src/includes)
set(my_impl_dir src/implementation)
set(test_dir tests)

add_executable(main 
${my_impl_dir}/main_noiaFattaBene.cpp
${my_impl_dir}/matricesModule.cpp
${my_impl_dir}/core.cpp
${my_impl_dir}/globals.cpp
${my_impl_dir}/less/IDRankPairLess.cpp
${my_impl_dir}/less/RankLess.cpp
${my_impl_dir}/less/TeamSetLess.cpp
${my_impl_dir}/hash/VisitedConfigSetHash.cpp
${my_impl_dir}/silly_strats/funcs.cpp
${my_impl_dir}/to_strings.cpp
${test_dir}/main_test.cpp)


target_compile_definitions(main 
PRIVATE 
$<$<CONFIG:Debug>:MIO_DEBUG>
$<$<CONFIG:Release>:MIO_RELEASE>
)

if (MSVC)
  add_link_options(
    $<$<CONFIG:Debug>:/INCREMENTAL> 
    $<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>
  )
endif()

set_target_properties(main PROPERTIES LINK_DEPENDS_NO_SHARED TRUE)

target_include_directories(main PRIVATE ${my_includes_dir} libs ${test_dir})

# Declare dependency on the absl::strings library
target_link_libraries(main absl::flat_hash_set absl::hash absl::btree)