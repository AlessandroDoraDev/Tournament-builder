#include <memory>
#include <functional>
#include "GLFW/glfw3.h"
void mainGui();


namespace SmallGui{
    void render(size_t width, size_t height, GLFWwindow* window);
}