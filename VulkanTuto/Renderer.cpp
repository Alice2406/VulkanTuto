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
	vulkanDraw.Init(vulkanDevice.GetQueueIndex(), vulkanDevice.GetDevice());
}

void Renderer::Destroy()
{
	//vulkanPipeline.Destroy();
	//vulkanInstance.Destroy();
	//vulkanDevice.Destroy();
	//vulkanSwapChain.Destroy();
}

//Render Doc -> Debug GPU
