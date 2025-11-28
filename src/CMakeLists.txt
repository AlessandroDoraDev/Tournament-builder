add_executable(${TB_MAIN_TARGET_NAME} WIN32
    implementation/main_noiaFattaBene.cpp
    implementation/TournamentBuilderGui.cpp
    implementation/WindowsDefs.cpp
)


target_compile_definitions(${TB_MAIN_TARGET_NAME} PRIVATE 
    $<$<CONFIG:Debug>:MIO_DEBUG>
    $<$<CONFIG:Release>:MIO_RELEASE>
)

set_target_properties(${TB_MAIN_TARGET_NAME} PROPERTIES LINK_DEPENDS_NO_SHARED TRUE)

if(${TB_CURRENT_COMPILER} STREQUAL COMPILER_MSVC)
    target_link_options(${TB_MAIN_TARGET_NAME} PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
endif()

target_include_directories(${TB_MAIN_TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${TB_CORELIB_INCLUDE_DIRS}
    ${TB_LIBS_INCLUDE_DIRS}
)


target_link_libraries(${TB_MAIN_TARGET_NAME} PRIVATE
    tournament-build-core-lib 
    imgui
    glfw
    stb
    nfd
    absl::flat_hash_set 
    absl::hash absl::btree
    absl::fixed_array
)



if(${TB_PLATFORM} STREQUAL WINDOWS)
    target_link_libraries(${TB_MAIN_TARGET_NAME}
        ole32
        uuid
        shell32
    )
endif()

if(${TB_PLATFORM} STREQUAL MACOSX)
    find_library(APPKIT_LIBRARY AppKit)
    find_library(UTI_LIBRARY UniformTypeIdentifiers)

    target_link_libraries(${TB_MAIN_TARGET_NAME} PRIVATE
        ${APPKIT_LIBRARY}
        ${UTI_LIBRARY}
    )
endif()


add_custom_command(
  TARGET ${TB_MAIN_TARGET_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/icons"
    "$<TARGET_FILE_DIR:${TB_MAIN_TARGET_NAME}>/icons"
)