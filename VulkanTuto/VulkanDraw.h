#pragma once
#include "Dependencies.h"

class VulkanDraw
{
public:
	void Init(uint32_t &queueIndex, vk::raii::Device& device);
	void Destroy();
private:
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	void createCommandPool(uint32_t &queueIndex, vk::raii::Device& device);
	void createCommandBuffer(vk::raii::Device& device);
	void recordCommandBuffer(uint32_t imageIndex);
};

