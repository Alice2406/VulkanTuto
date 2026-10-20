#pragma once
#include "Dependencies.h"

class VulkanDevice;
class VulkanSwapChain;
class VulkanPipeline;

class VulkanDraw
{
public:
	void Init(VulkanDevice& device,VulkanSwapChain& swapChain,VulkanPipeline& pipeline);
	void drawFrame();
	void Destroy();
private:
	VulkanDevice* vulkanDevice = nullptr;
	VulkanSwapChain* vulkanSwapChain = nullptr;
	VulkanPipeline* vulkanPipeline = nullptr;

	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	vk::raii::Semaphore presentCompleteSemaphore = nullptr;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	vk::raii::Fence drawFence = nullptr;

	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();
	void recordCommandBuffer(uint32_t imageIndex);
	void transition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask);
};

