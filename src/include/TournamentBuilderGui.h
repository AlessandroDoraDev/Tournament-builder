#pragma once
#include <memory>
#include <functional>
#include <filesystem>
#include <vector>
#include <cstddef>
#include <regex>
#include <string>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "TournamentConfig.h"
#include "Player.h"
#include <utility>
#include "osMacro.h"
#include "appImgDefs.h"

void mainGui();

namespace SmallGui{
    const ::std::size_t INPUT_WINDOW_WIDTH=1280/2;
    const ::std::size_t INPUT_WINDOW_HEIGHT=1280/4;
    inline ::std::size_t result_window_width=0;
    inline ::std::size_t result_window_height=0;
#if defined(OS_LINUX) && defined(APPIMAGE_BUILD)
    std::filesystem::path iconsPath();
    const std::regex ICONS_FOLDER_PATTERN(R"(^\d+x\d+$)");
#else
    std::filesystem::path iconsPath(){return "icons/";}
#endif
    const std::regex icon_pattern(R"(^icon\d+x\d+\.png$)");
    void render(GLFWwindow* window);
    std::pair<std::string, std::string> playerToCellString(const Player&);
    ::std::vector<::std::filesystem::path> listMatchingFiles(const ::std::filesystem::path& dir, const ::std::regex& pattern);
    bool browseCSVInteraction(GLFWwindow* window, std::string* dest, const std::string& defaultPath);
    bool browseFolderInteraction(GLFWwindow* window, std::string* dest, const std::string& defaultPath);
};