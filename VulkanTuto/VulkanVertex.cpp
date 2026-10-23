#include "VulkanVertex.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

void VulkanVertex::Init(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
	createVertexBuffer(device, physicalDevice, commandPool, queue);
	createIndexBuffer(device, physicalDevice, commandPool, queue);
	createUniformBuffers(device, physicalDevice);
}

void VulkanVertex::createVertexBuffer(vk::raii::Device &device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
	std::cout << "Creating vertex buffer..." << std::endl;
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	auto [stagingBuffer, stagingBufferMemory] =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, physicalDevice);

	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, vertices.data(), bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(vertexBuffer, vertexBufferMemory) =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, device, physicalDevice);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize, commandPool, device, queue);
}

void VulkanVertex::createIndexBuffer(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	auto [stagingBuffer, stagingBufferMemory] =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, physicalDevice);

	void* data = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(data, indices.data(), (size_t)bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(indexBuffer, indexBufferMemory) =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, device, physicalDevice);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize, commandPool, device, queue);
}

void VulkanVertex::createUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice)
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		auto [buffer, bufferMemory] =
			createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, device, physicalDevice);
		uniformBuffers.emplace_back(std::move(buffer));
		uniformBufferMemory.emplace_back(std::move(bufferMemory));
		uniformBuffersMapped.emplace_back(uniformBufferMemory.back().mapMemory(0, bufferSize));
	}
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> VulkanVertex::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice)
{
	vk::BufferCreateInfo bufferInfo{
	   .size = size,
	   .usage = usage,
	   .sharingMode = vk::SharingMode::eExclusive
	};
	vk::raii::Buffer buffer(device, bufferInfo);
	vk::MemoryRequirements memRequirements =
		buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocateInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(
			memRequirements.memoryTypeBits,
			properties,
			physicalDevice
		)
	};
	vk::raii::DeviceMemory bufferMemory(device, memoryAllocateInfo);
	buffer.bindMemory(*bufferMemory, 0);
	return {
		std::move(buffer),
		std::move(bufferMemory)
	};
}

uint32_t VulkanVertex::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::raii::PhysicalDevice& physicalDevice)
{
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		if ((typeFilter & (1u << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Failed to find suitable memory type!");
}

void VulkanVertex::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, vk::raii::CommandPool& commandPool, vk::raii::Device& device, vk::raii::Queue& queue)
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	vk::raii::CommandBuffer commandCopyBuffer = std::move(device.allocateCommandBuffers(allocInfo).front());
	commandCopyBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();
	queue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
	queue.waitIdle();
}



