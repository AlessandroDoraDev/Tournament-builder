set(test_dir ${CMAKE_CURRENT_SOURCE_DIR})

# Create the test executable
add_executable(test_target
    ${test_dir}/main_test.cpp
    # Add other test source files here if needed
)

# Add include directories
target_include_directories(test_target
    PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/gtest/googletest/include
    ${CMAKE_SOURCE_DIR}/libs/gtest/googlemock/include
    ${CMAKE_SOURCE_DIR}/${my_includes_dir}
    ${test_dir}
)

# Link against Google Test and other required libraries
target_link_libraries(test_target 
    PRIVATE
    GTest::gtest_main
    absl::flat_hash_set 
    absl::hash 
    absl::btree
)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(test_target)