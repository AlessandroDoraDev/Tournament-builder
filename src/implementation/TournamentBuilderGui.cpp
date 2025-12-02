#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "TournamentBuilderGui.h"
#include <string>
#include <print>
#include "imgui_stdlib.h"
#include "stb_image.h"
#include "TournamentBuilderAPI.h"
#include <format>
#include "globals.h"
#include "nfd.h"
#include <array>
#include "osMacro.h"

#ifdef OS_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(OS_LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
    //unsupported wayland (accordingly to NFDe's doc) until library update, but defining in case the lib gets updated. 
    //The wayland support right now doesn't do anything anyway, accordingly to NFDe's documentation.
    #define GLFW_EXPOSE_NATIVE_WAYLAND 
#elif defined(OS_MACOS)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <nfd_glfw3.h>


bool accept_player_list_path;
bool accept_result_directory_path;
bool accept_rank_list_path;
static std::string player_list_path;
static std::string rank_list_path;
static std::string result_directory=std::filesystem::current_path().string();


static void glfw_error_callback(int error, const char* description){
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#if defined(OS_LINUX) && defined(APPIMAGE_BUILD)

std::filesystem::path SmallGui::iconsPath(){return APPDIR/"../share/icons/hicolor/";}

#elifndef OS_WINDOWS

std::filesystem::path SmallGui::iconsPath(){return "icons/";}

#endif

void mainGui(){
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    if(NFD_Init()==NFD_ERROR){
        return;
    }

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Essential for macOS
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "tournament-builder");
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "tournament-builder");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "tournament-builder-instance");
    
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    
    GLFWwindow* window = glfwCreateWindow(
        (int)(SmallGui::INPUT_WINDOW_WIDTH * main_scale), 
        (int)(SmallGui::INPUT_WINDOW_HEIGHT * main_scale), 
        "Tournament builder", 
        nullptr, 
        nullptr
    );
    glfwSetWindowSizeLimits(window, 
        SmallGui::INPUT_WINDOW_WIDTH, SmallGui::INPUT_WINDOW_HEIGHT,
        SmallGui::INPUT_WINDOW_WIDTH, SmallGui::INPUT_WINDOW_HEIGHT
    );

    if (window == nullptr)
        return;
    

#ifndef  OS_WINDOWS
    std::vector<std::filesystem::path> icon_paths=SmallGui::listMatchingFiles(SmallGui::iconsPath(), SmallGui::icon_pattern);
    std::vector<GLFWimage> icons;
    icons.reserve(icon_paths.size());
    if(icon_paths.size()==0){
        std::println("Didn't find app's icons");
    }
    for(std::filesystem::path& path: icon_paths){
        int width, height, desired_channels;
        unsigned char* pixels= stbi_load(path.string().c_str(), &width, &height, &desired_channels, 4);
        if(pixels){
            icons.emplace_back(width, height, pixels);
        }else{
            std::println("Problem loading icon: {}, {}", path.string(), stbi_failure_reason());
        }
    }
#endif    

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetDropCallback(window, 
        [] (GLFWwindow* wnd, int count, const char* paths[]) {
        if (count > 0) {
            if(accept_player_list_path){
                player_list_path= paths[0];
                player_list_path+='\0';
            }
            if(accept_rank_list_path){
                rank_list_path= paths[0];
                rank_list_path+='\0';
            }
            if(accept_result_directory_path){
                result_directory= paths[0];
                result_directory= '\0';
            }
        }
    });

    
#ifndef  OS_WINDOWS
    glfwSetWindowIcon(window, icons.size(), icons.data());
    
    for(GLFWimage& icon: icons){
        stbi_image_free(icon.pixels);
    }
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename= nullptr;
    
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0){
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
           
        SmallGui::render(window);
        
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    NFD_Quit();
    glfwTerminate();
}


void SmallGui::render(GLFWwindow* window){
    static bool caught_nonexistingfile_error_playerlist=false;
    static bool caught_nonexistingfile_error_ranklist=false;
    static bool caught_nonexistingfile_error_result_dir=false;
    static bool build_button_pressed;
    static bool tournament_build_error=false;
    static bool show_done_msg=false;
    static std::string tournament_build_error_msg="";
    static int n_players=0;
    static int rotation_size=3;
    static const float BRWOSE_BUTTON_WIDTH=ImGui::CalcTextSize("Browse").x+ImGui::GetStyle().FramePadding.x*2;

    build_button_pressed=false;
    accept_player_list_path=false;
    accept_rank_list_path=false;
    accept_result_directory_path=false;
    

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    
    ImGui::Begin("Tournament Builder", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);
        
        
    ImGui::InputText("Player list path", &player_list_path);
    if(ImGui::IsItemHovered()){
        accept_player_list_path=true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+ImGui::GetContentRegionAvail().x-BRWOSE_BUTTON_WIDTH);
    if(ImGui::Button("Browse##1")){
        SmallGui::browseCSVInteraction(window, &player_list_path, result_directory);
    }
    ImGui::NewLine();
    ImGui::InputText("Rank list path", &rank_list_path);
    if(ImGui::IsItemHovered()){
        accept_rank_list_path=true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+ImGui::GetContentRegionAvail().x-BRWOSE_BUTTON_WIDTH);
    if(ImGui::Button("Browse##2")){
        SmallGui::browseCSVInteraction(window, &rank_list_path, result_directory);
    }
    ImGui::NewLine();
    std::string out_dir_txt="Output directory";
    ImGui::InputText(out_dir_txt.c_str(), &result_directory);
    float input_texts_width=ImGui::GetItemRectSize().x-ImGui::CalcTextSize((out_dir_txt+" ").c_str()).x;
    if(ImGui::IsItemHovered()){
        accept_result_directory_path=true;
    }
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+ImGui::GetContentRegionAvail().x-BRWOSE_BUTTON_WIDTH);
    if(ImGui::Button("Browse##3")){
        SmallGui::browseFolderInteraction(window, &result_directory, result_directory);
    }
    ImGui::NewLine();
    float num_input_size=ImGui::CalcTextSize("xxx ").x;
    if(ImGui::BeginChild("##NumBox", 
        ImVec2(input_texts_width, 0),
        ImGuiChildFlags_AutoResizeY,
        ImGuiWindowFlags_NoScrollWithMouse
    )){
        if(ImGui::BeginChild("##Box1",
            ImVec2(input_texts_width/2, 0),
            ImGuiChildFlags_AutoResizeY
        )){
            ImGui::Text("Players per team");
            ImGui::NewLine();
            ImGui::PushItemWidth(num_input_size);
            ImGui::InputInt("##", &n_players, 0);
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if(ImGui::BeginChild("##Box2",
            ImVec2(input_texts_width/2, 0),
            ImGuiChildFlags_AutoResizeY
        )){
            ImGui::Text("Algorithm pace");
            ImGui::NewLine();
            ImGui::PushItemWidth(num_input_size);
            ImGui::InputInt("##", &rotation_size, 0);
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
    }
    ImGui::NewLine();
    ImGui::EndChild();
    if(ImGui::Button("Build this tournament!!")){
        show_done_msg=false;
        if(!player_list_path.empty()){
            if(std::filesystem::exists(player_list_path)){
                caught_nonexistingfile_error_playerlist=false;
            }else{
                caught_nonexistingfile_error_playerlist=true;
            }
        }
        if(!rank_list_path.empty()){
            if(std::filesystem::exists(rank_list_path)){
                caught_nonexistingfile_error_ranklist=false;
            }else{
                caught_nonexistingfile_error_ranklist=true;
            }
        }
        if(!result_directory.empty()){
            if(std::filesystem::exists(result_directory)){
                caught_nonexistingfile_error_result_dir=false;
            }else{
                caught_nonexistingfile_error_result_dir=true;
            }
        }
        build_button_pressed=true;
    }
    if(caught_nonexistingfile_error_playerlist){
        ImGui::NewLine();
        ImGui::Text("Player list file does not exist");
    }
    
    if(caught_nonexistingfile_error_ranklist){
        ImGui::NewLine();
        ImGui::Text("Rank list file does not exist");
    }

    if(caught_nonexistingfile_error_result_dir){
        ImGui::NewLine();
        ImGui::Text("Directory specified to output the results does not exist");
    }
    
    if( build_button_pressed
        && !caught_nonexistingfile_error_ranklist && !player_list_path.empty()
        && !caught_nonexistingfile_error_playerlist && !rank_list_path.empty()
        && !caught_nonexistingfile_error_result_dir && !result_directory.empty()){
        BuildTournamentResult build_res= buildTournamentFromCSV(player_list_path, rank_list_path, n_players, rotation_size);
        if(build_res.error_code!=GOOD_TOURNAMENT_RESULT){
            tournament_build_error_msg.clear();
            tournament_build_error_msg=
            ERROR_MESSAGE_MAP.at(build_res.error_code)
            +"\nIf you can't find the cause of this issue, submit a ticket or an issue to the software mantainer.";
            tournament_build_error=true;
        }else{
            tournament_build_error=false;
            std::filesystem::path r_path(result_directory);
            r_path.append(std::filesystem::path(player_list_path).stem().string()+".html");
            build_res.tournament_config.genHTMLTable(r_path.string());
            show_done_msg=true;
        }
    }
    if(tournament_build_error){
        ImGui::NewLine();
        ImGui::TextWrapped("%s", tournament_build_error_msg.c_str());
    }
    if(show_done_msg){ //double check. No need
        ImGui::NewLine();
        ImGui::TextWrapped("Tournament built!!");
    }
    
    ImGui::End();
}


std::pair<std::string, std::string> SmallGui::playerToCellString(const Player& p){
    return std::make_pair(p.name, ENUM_RANK_TO_STRING_MAP.at(p.rank));
}

#ifndef OS_WINDOWS
std::vector<std::filesystem::path> 
SmallGui::listMatchingFiles(const std::filesystem::path& dir, const std::regex& pattern){
    std::vector<std::filesystem::path> results;
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
        return results;

#if defined(OS_LINUX) && defined(APPIMAGE_BUILD)
    for(auto const& entry : std::filesystem::directory_iterator(dir)){
        if (entry.is_regular_file())
            continue;
        const auto dirname= entry.path().filename().string();
        if(std::regex_match(dirname, SmallGui::ICONS_FOLDER_PATTERN)){
            results.push_back(entry.path()/"apps/icon.png");
        }
    }
#else
    for (auto const& entry : std::filesystem::directory_iterator(dir)){
        if (!entry.is_regular_file())
            continue;
        const auto filename = entry.path().filename().string();
        if (std::regex_match(filename, pattern)){
            results.push_back(entry.path());
        }
    }
#endif

    return results;
}
#endif

bool SmallGui::browseCSVInteraction(GLFWwindow* window, std::string* dest, const std::string& defaultPath){
    bool res=true;
    
    static const std::size_t BRWOSE_CSV_FILTER_COUNT= 1; 
    nfdu8char_t* outPath;
    std::array<nfdu8filteritem_t, BRWOSE_CSV_FILTER_COUNT> filters = {{ "CSV", "csv" }};
    nfdopendialogu8args_t args= {0};
    args.filterList= filters.data();
    args.filterCount= BRWOSE_CSV_FILTER_COUNT;
    args.defaultPath= defaultPath.data();
    NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);
    nfdresult_t result= NFD_OpenDialogU8_With(&outPath, &args);
    if (result==NFD_OKAY){
        dest->assign(outPath);
        NFD_FreePathU8(outPath);
    }else if(result!=NFD_CANCEL){
        std::print("{}", NFD_GetError());
        res=false;
    }
    return res;
}


bool SmallGui::browseFolderInteraction(GLFWwindow* window, std::string* dest, const std::string& defaultPath){
    bool res=true; 
    nfdu8char_t* outPath;
    nfdpickfolderu8args_t args= {0};
    args.defaultPath=defaultPath.data();
    NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);
    nfdresult_t result= NFD_PickFolderU8_With(&outPath, &args);
    if (result==NFD_OKAY){
        dest->assign(outPath);
        NFD_FreePathU8(outPath);
    }else if(result!=NFD_CANCEL){
        std::print("{}", NFD_GetError());
        res=false;
    }
    return res;
}
