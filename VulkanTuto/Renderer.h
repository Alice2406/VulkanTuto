#pragma once
#include "Dependencies.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanDraw.h"
#include "VulkanVertex.h"
#include "VulkanDescriptor.h"
#include "VulkanTextures.h"
#include "VulkanDepth.h"

class Renderer
{
public:
	void Init(GLFWwindow* window);
	void DrawFrame(GLFWwindow* window);
	VulkanDraw& GetVulkanDraw() { return vulkanDraw; }
	void WaitIdle();
	void Destroy();

private:
	VulkanInstance vulkanInstance;
	VulkanDevice vulkanDevice;
	VulkanSwapChain vulkanSwapChain;
	VulkanPipeline vulkanPipeline;
	VulkanDraw vulkanDraw;
	VulkanVertex vulkanVertex;
	VulkanDescriptor vulkanDescriptor;
	VulkanTextures vulkanTextures;
	VulkanDepth vulkanDepth;
};

