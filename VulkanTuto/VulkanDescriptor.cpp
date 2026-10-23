#include "VulkanDescriptor.h"
#include "Vertex.h"
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

void VulkanDescriptor::Init(vk::raii::Device &device, vk::raii::DescriptorSetLayout &descriptorSetLayout, std::vector<vk::raii::Buffer>& uniformBuffers)
{
	createDescriptorPool(device);
	createDescriptorSets(descriptorSetLayout, device, uniformBuffers);
}

void VulkanDescriptor::createDescriptorPool(vk::raii::Device &device)
{
	vk::DescriptorPoolSize poolSize{
		.type = vk::DescriptorType::eUniformBuffer,
		.descriptorCount = MAX_FRAMES_IN_FLIGHT
	};
	vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = MAX_FRAMES_IN_FLIGHT,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize
	};
	descriptorPool = vk::raii::DescriptorPool(device, poolInfo);	
}

void VulkanDescriptor::createDescriptorSets(vk::raii::DescriptorSetLayout &descriptorSetLayout, vk::raii::Device &device, std::vector<vk::raii::Buffer>& uniformBuffers)
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{
		.descriptorPool = descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		.pSetLayouts = layouts.data()
	};
	descriptorSets = device.allocateDescriptorSets(allocInfo);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{
			.buffer = uniformBuffers[i],
			.offset = 0,
			.range = sizeof(UniformBufferObject)
		};
		vk::WriteDescriptorSet descriptorWrite{
			.dstSet = descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &bufferInfo
		};
		device.updateDescriptorSets({ descriptorWrite }, {});
	}
}


