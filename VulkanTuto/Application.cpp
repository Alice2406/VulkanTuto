#include "Application.h"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

void Application::Init(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
    InitWindow();
    InitVulkan(vertices, indices);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer.DrawFrame(window);
    }
    renderer.WaitIdle();
}

void Application::Destroy()
{
    
    DestroyWindow();
    DestroyVulkan();
}

void Application::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "hello", nullptr, nullptr);
    glfwSetWindowUserPointer(window, &renderer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Application::InitVulkan(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
    renderer.Init(window, vertices, indices);
}

void Application::DestroyWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::DestroyVulkan()
{
	renderer.WaitIdle();
}
