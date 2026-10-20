#include "VulkanDraw.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"

void VulkanDraw::Init(uint32_t &queueIndex, vk::raii::Device& device)
{
	createCommandPool(queueIndex, device);
	createCommandBuffer(device);
}

void VulkanDraw::createCommandPool(uint32_t &queueIndex, vk::raii::Device& device)
{
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueIndex
	};
	commandPool = vk::raii::CommandPool(device, poolInfo);

}

void VulkanDraw::createCommandBuffer(vk::raii::Device& device)
{
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());
}

void VulkanDraw::recordCommandBuffer(uint32_t imageIndex)
{
	commandBuffer.begin({});
}
