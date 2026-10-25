#include "VulkanDescriptor.h"
#include "Vertex.h"
#include <iostream>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

void VulkanDescriptor::Init(vk::raii::Device &device, vk::raii::DescriptorSetLayout &descriptorSetLayout, std::vector<vk::raii::Buffer>& uniformBuffers, vk::raii::ImageView &textureImageView, vk::raii::Sampler &textureSampler)
{
	createDescriptorPool(device);
	createDescriptorSets(descriptorSetLayout, device, uniformBuffers, textureImageView, textureSampler);
}

void VulkanDescriptor::createDescriptorPool(vk::raii::Device &device)
{
	std::cout << "Creating descriptor pool..." << std::endl;
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {
			vk::DescriptorPoolSize{
				.type = vk::DescriptorType::eUniformBuffer,
				.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
			},
			vk::DescriptorPoolSize{
				.type = vk::DescriptorType::eCombinedImageSampler,
				.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
			}
	};
	vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.data()
	};
	descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
}

void VulkanDescriptor::createDescriptorSets(vk::raii::DescriptorSetLayout &descriptorSetLayout, vk::raii::Device &device, std::vector<vk::raii::Buffer>& uniformBuffers, vk::raii::ImageView &textureImageView, vk::raii::Sampler &textureSampler)
{
	std::cout << "Creating descriptor sets..." << std::endl;
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{
		.descriptorPool = descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		.pSetLayouts = layouts.data()
	};
	descriptorSets = device.allocateDescriptorSets(allocInfo);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{ .buffer = uniformBuffers[i], .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo  imageInfo{ .sampler = textureSampler, .imageView = textureImageView, .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites{ { { .dstSet = descriptorSets[i],
														 .dstBinding = 0,
														 .dstArrayElement = 0,
														 .descriptorCount = 1,
														 .descriptorType = vk::DescriptorType::eUniformBuffer,
														 .pBufferInfo = &bufferInfo },
														{ .dstSet = descriptorSets[i],
														 .dstBinding = 1,
														 .dstArrayElement = 0,
														 .descriptorCount = 1,
														 .descriptorType = vk::DescriptorType::eCombinedImageSampler,
														 .pImageInfo = &imageInfo } } };
		device.updateDescriptorSets(descriptorWrites, {});
	}
}


