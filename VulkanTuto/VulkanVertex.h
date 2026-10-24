#pragma once
#include "Dependencies.h"
#include "Vertex.h"

class VulkanVertex
{
public:
	void Init(vk::raii::Device &device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);
	vk::raii::Buffer& GetVertexBuffer() { return vertexBuffer; }
	vk::raii::Buffer& GetIndexBuffer() { return indexBuffer; }
	std::size_t GetVertexCount() { return vertices.size(); }
	std::size_t GetIndicesCount() { return indices.size(); }
	std::vector<void*>& GetUniformBuffersMapped() { return uniformBuffersMapped; }
	std::vector<vk::raii::Buffer>& GetUniformBuffers() { return uniformBuffers; }
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::raii::PhysicalDevice& physicalDevice);
	vk::raii::CommandBuffer beginSingleTimeCommands(vk::raii::CommandPool& commandPool, vk::raii::Device& device);
	void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer, vk::raii::Queue& queue);
	void Destroy();

private:
	const std::vector<Vertex> vertices = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, 0.5f, -0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }
	};
	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	void createUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice);
	void createVertexBuffer(vk::raii::Device &device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);
	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, vk::raii::CommandPool& commandPool, vk::raii::Device& device, vk::raii::Queue& queue);
	void createIndexBuffer(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);

	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer       indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;

	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBufferMemory;
	std::vector<void*> uniformBuffersMapped;
};

