cmake_minimum_required(VERSION 3.16)

project(my_project)

set(CMAKE_CXX_STANDARD 20)

# Process Abseil's CMake build system
add_subdirectory(abseil-cpp)

add_executable(main 
main_noiaFattaBene.cpp
matricesModule.cpp
core.cpp)


target_compile_definitions(main 
PRIVATE 
$<$<CONFIG:Debug>:MIO_DEBUG>
$<$<CONFIG:Release>:MIO_RELEASE>
)

target_include_directories(main PRIVATE .)

# Declare dependency on the absl::strings library
target_link_libraries(main absl::flat_hash_set absl::hash absl::btree)