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

	void Init(GLFWwindow* window, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	void DrawFrame(GLFWwindow* window);
	VulkanDraw& GetVulkanDraw() { return vulkanDraw; }
	void WaitIdle();

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

