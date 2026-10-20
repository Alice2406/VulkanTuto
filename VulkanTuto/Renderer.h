#pragma once
#include "Dependencies.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanDraw.h"

class Renderer
{
public:
	void Init(GLFWwindow* window);
	void DrawFrame();
	void WaitIdle();
	void Destroy();

private:
	VulkanInstance vulkanInstance;
	VulkanDevice vulkanDevice;
	VulkanSwapChain vulkanSwapChain;
	VulkanPipeline vulkanPipeline;
	VulkanDraw vulkanDraw;
};

