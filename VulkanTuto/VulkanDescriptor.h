#pragma once
#include "Dependencies.h"

class VulkanDescriptor
{
public :

	void Init(vk::raii::Device &device, vk::raii::DescriptorSetLayout &descriptorSetLayout, std::vector<vk::raii::Buffer>& uniformBuffers, vk::raii::ImageView &textureImageView, vk::raii::Sampler &textureSampler);
	std::vector<vk::raii::DescriptorSet>& GetDescriptorSets() { return descriptorSets; }

private :

	vk::raii::DescriptorPool descriptorPool = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptorSets;

	void createDescriptorPool(vk::raii::Device &device);
	void createDescriptorSets(vk::raii::DescriptorSetLayout &descriptorSetLayout, vk::raii::Device &device, std::vector<vk::raii::Buffer>& uniformBuffers, vk::raii::ImageView &textureImageView, vk::raii::Sampler &textureSampler);
};

