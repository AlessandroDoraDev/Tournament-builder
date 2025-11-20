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


bool accept_player_list_path;
bool accept_result_directory_path;
bool accept_rank_list_path;
static std::string player_list_path;
static std::string rank_list_path;
static std::string result_directory=std::filesystem::current_path().string();


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
void mainGui(){
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    
    GLFWwindow* window = glfwCreateWindow(
        (int)(SmallGui::INPUT_WINDOW_WIDTH * main_scale), 
        (int)(SmallGui::INPUT_WINDOW_HEIGHT * main_scale), 
        "Tournament builder", 
        nullptr, 
        nullptr
    );
    if (window == nullptr)
        return;
    std::vector<std::filesystem::path> icon_paths=SmallGui::listMatchingFiles(SmallGui::icons_path, SmallGui::icon_pattern);
    std::vector<GLFWimage> icons;
    icons.reserve(5);
    for(std::filesystem::path& path: icon_paths){
        int width, height, desired_channels;
        unsigned char* pixels= stbi_load(path.string().c_str(), &width, &height, &desired_channels, 4);
        icons.emplace_back(width, height, pixels);
    }
    
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
    
    glfwSetWindowIcon(window, icons.size(), icons.data());
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
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
    ImGui::NewLine();
    ImGui::InputText("Rank list path", &rank_list_path);
    if(ImGui::IsItemHovered()){
        accept_rank_list_path=true;
    }
    ImGui::NewLine();
    std::string out_dir_txt="Output directory";
    ImGui::InputText(out_dir_txt.c_str(), &result_directory);
    float input_texts_width=ImGui::GetItemRectSize().x-ImGui::CalcTextSize((out_dir_txt+" ").c_str()).x;
    if(ImGui::IsItemHovered()){
        accept_rank_list_path=true;
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
        && !caught_nonexistingfile_error_playerlist && !rank_list_path.empty()){
        BuildTournamentResult build_res= buildTournamentFromCSV(player_list_path, rank_list_path, n_players, rotation_size);
        if(build_res.error_code!=GOOD_TOURNAMENT_RESULT){
            tournament_build_error_msg.clear();
            tournament_build_error_msg=
            ERROR_MESSAGE_MAP.at(build_res.error_code)
            +"\nIf you can't find the cause of this issue, submit a ticket or an issue to the software mantainer.";
            tournament_build_error=true;
        }else{
            tournament_build_error=false;
            build_res.tournament_config;
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


std::vector<std::filesystem::path> 
SmallGui::listMatchingFiles(const std::filesystem::path& dir, const std::regex& pattern){
    std::vector<std::filesystem::path> results;
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
        return results;

    for (auto const& entry : std::filesystem::directory_iterator(dir)){
        if (!entry.is_regular_file())
            continue;
        const auto filename = entry.path().filename().string();
        if (std::regex_match(filename, pattern)){
            results.push_back(entry.path());
        }
    }

    return results;
}
