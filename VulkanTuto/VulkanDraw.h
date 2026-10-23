#pragma once
#include "Dependencies.h"

class VulkanDevice;
class VulkanSwapChain;
class VulkanPipeline;
class VulkanVertex;
class VulkanDescriptor;

class VulkanDraw
{
public:

	void Init(VulkanDevice& device,VulkanSwapChain& swapChain,VulkanPipeline& pipeline,VulkanVertex& vertex,VulkanDescriptor& descriptor, GLFWwindow* window);
	void InitCommandBuffers();
	vk::raii::CommandPool& GetCommandPool() { return commandPool; }
	void drawFrame(GLFWwindow* window);
	void Destroy();
	bool framebufferResized = false;

private:

	VulkanDevice* vulkanDevice = nullptr;
	VulkanSwapChain* vulkanSwapChain = nullptr;
	VulkanPipeline* vulkanPipeline = nullptr;
	VulkanVertex* vulkanVertex = nullptr;
	VulkanDescriptor* vulkanDescriptor = nullptr;

	uint32_t frameIndex = 0;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void recordCommandBuffer(uint32_t imageIndex);
	void updateUniformBuffer(uint32_t imageIndex);
	void transition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask);
};

