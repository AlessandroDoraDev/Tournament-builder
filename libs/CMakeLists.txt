add_subdirectory(abseil-cpp)
add_subdirectory(nativefiledialog-extended)


include(CTest)
enable_testing()
add_subdirectory(gtest)


set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
add_subdirectory(GLFW)


add_library(${TB_IMGUI_TARGET_NAME} STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(${TB_IMGUI_TARGET_NAME} PRIVATE
    ./imgui
    imgui/backends
    imgui/misc
    GLFW/include
)
target_link_libraries(${TB_IMGUI_TARGET_NAME}
    glfw
)


add_library(${TB_STB_TARGET_NAME} STATIC
    stb/stb_image.cpp
)
target_include_directories(${TB_STB_TARGET_NAME} PRIVATE
    ./stb
)
