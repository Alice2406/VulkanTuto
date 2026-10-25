#include "Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

void Renderer::Init(GLFWwindow* window, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
	vulkanInstance.CreateInstance();
	vulkanDevice.Init(vulkanInstance.GetInstance(), window);
	vulkanSwapChain.Init(vulkanDevice.GetPhysicalDevice(), vulkanDevice.GetDevice(), vulkanDevice.GetSurface(), window);
	vulkanPipeline.Init(vulkanDevice.GetDevice(), vulkanSwapChain.GetSurfaceFormat(), vulkanDevice.GetPhysicalDevice(), vulkanDepth);
	vulkanDraw.Init(vulkanDevice, vulkanSwapChain, vulkanPipeline, vulkanVertex, vulkanDescriptor, vulkanDepth, window);
	vulkanDepth.Init(vulkanDevice.GetPhysicalDevice(), vulkanSwapChain.GetSwapChainExtent(), vulkanTextures, vulkanDevice.GetDevice());
	vulkanTextures.Init(vulkanVertex, vulkanDevice.GetDevice(), vulkanDevice.GetPhysicalDevice(), vulkanDraw.GetCommandPool(), vulkanDevice.GetQueue());
	vulkanVertex.Init(vulkanDevice.GetDevice(), vulkanDevice.GetPhysicalDevice(), vulkanDraw.GetCommandPool(), vulkanDevice.GetQueue(), vertices, indices);
	vulkanDescriptor.Init(vulkanDevice.GetDevice(), vulkanPipeline.GetDescriptorSetLayout(), vulkanVertex.GetUniformBuffers(), vulkanTextures.getTextureImageView(), vulkanTextures.getTextureSampler());
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