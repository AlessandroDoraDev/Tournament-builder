#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "TournamentBuilderGui.h"
#include <string>
#include <print>
#include <filesystem>
#include "imgui_stdlib.h"
#include "stb_image.h"
#include <regex>
#include "TournamentBuilderAPI.h"

static const size_t WINDOW_WIDTH=1280/2;
static const size_t WINDOW_HEIGHT=800/4;
static const std::string icons_path="../../../../assets";
static const std::regex icon_pattern(R"(^icon\d+x\d+\.png$)");

std::vector<std::filesystem::path> 
listMatchingFiles(const std::filesystem::path& dir, const std::regex& pattern){
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

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
void mainGui()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(WINDOW_WIDTH * main_scale), (int)(WINDOW_HEIGHT * main_scale), "Tournament builder", nullptr, nullptr);
    if (window == nullptr)
        return;
    std::vector<std::filesystem::path> icon_paths=listMatchingFiles(icons_path, icon_pattern);
    std::vector<GLFWimage> icons;
    icons.reserve(5);
    for(std::filesystem::path& path: icon_paths){
        int width, height, desired_channels;
        unsigned char* pixels= stbi_load(path.string().c_str(), &width, &height, &desired_channels, 4);
        icons.emplace_back(width, height, pixels);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetWindowIcon(window, icons.size(), icons.data());
    // for(GLFWimage& icon: icons){
    //     stbi_image_free(icon.pixels);
    // }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
/*
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
*/
        
        SmallGui::render(WINDOW_WIDTH, WINDOW_HEIGHT, window);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}



void SmallGui::render(size_t width, size_t height, GLFWwindow* window){
    static std::string droppedFilePath;
    static bool caught_nonexistingfile_error=false;
    glfwSetDropCallback(window, [](GLFWwindow* wnd, int count, const char* paths[]) {
        if (count > 0) {
            droppedFilePath = paths[0];
            droppedFilePath+='\0';
        }
    });
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    ImGui::Begin("MyWindow", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);


    ImGui::InputText("Path", &droppedFilePath);
    ImGui::NewLine();
    if(ImGui::Button("Build this tournament!!")&&!droppedFilePath.empty()){
        if(std::filesystem::exists(droppedFilePath)){
            caught_nonexistingfile_error=false;
            //buildTournamentFromCSV(droppedFilePath);
        }else{
            caught_nonexistingfile_error=true;
        }
    }
    if(caught_nonexistingfile_error){
        ImGui::NewLine();
        ImGui::Text("File does not exist");
    }
    
    ImGui::End();
}