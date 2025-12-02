add_executable(${TB_TESTS_TARGET_NAME}
    main_test.cpp
    utils.cpp
    ../src/implementation/WindowsDefs.cpp
)

target_include_directories(${TB_TESTS_TARGET_NAME} PRIVATE
    ${TB_GTEST_INCLUDE_DIRS}
)

target_link_libraries(${TB_TESTS_TARGET_NAME} PRIVATE
    GTest::gtest_main
    absl::flat_hash_set 
    absl::hash 
    absl::btree
    tournament-build-core-lib
)

add_custom_command(
    TARGET ${TB_TESTS_TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${TB_TESTS_TARGET_NAME}>/assets"
)

include(GoogleTest)
gtest_discover_tests(${TB_TESTS_TARGET_NAME})