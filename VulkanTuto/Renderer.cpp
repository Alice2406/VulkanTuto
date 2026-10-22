#include "Renderer.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"

void Renderer::Init(GLFWwindow* window)
{
	vulkanInstance.CreateInstance();
	vulkanDevice.Init(vulkanInstance.GetInstance(), window);
	vulkanSwapChain.Init(vulkanDevice.GetPhysicalDevice(), vulkanDevice.GetDevice(), vulkanDevice.GetSurface(), window);
	vulkanPipeline.Init(vulkanDevice.GetDevice(), vulkanSwapChain.GetSurfaceFormat());
	vulkanDraw.Init(vulkanDevice, vulkanSwapChain, vulkanPipeline, vulkanVertex, window);
	vulkanVertex.Init(vulkanDevice.GetDevice(), vulkanDevice.GetPhysicalDevice(), vulkanDraw.GetCommandPool(), vulkanDevice.GetQueue());
	vulkanDraw.InitCommandBuffers();
}

void Renderer::DrawFrame(GLFWwindow* window)
{
	vulkanDraw.drawFrame(window);
}

void Renderer::WaitIdle()
{
	vulkanDevice.GetDevice().waitIdle();
}

void Renderer::Destroy()
{
}

//Render Doc -> Debug GPU
