#pragma once
#include "Dependencies.h"

class VulkanTextures;

class VulkanDepth
{
public:
	void Init(vk::raii::PhysicalDevice &physicalDevice, vk::Extent2D swapChainExtent, VulkanTextures &textures, vk::raii::Device &device);
	vk::raii::ImageView& GetDepthImageView() { return depthImageView; }
	vk::raii::Image& GetDepthImage() { return depthImage; }
	vk::Format findDepthFormat(vk::raii::PhysicalDevice& physicalDevice);
	void createDepthResources(vk::raii::PhysicalDevice &physicalDevice, vk::Extent2D swapChainExtent, vk::raii::Device &device);
	void Destroy();
private :
	VulkanTextures* vulkanTextures = nullptr;
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, vk::raii::PhysicalDevice &physicalDevice);
	vk::raii::Image depthImage = nullptr;
	vk::raii::DeviceMemory depthImageMemory = nullptr;
	vk::raii::ImageView depthImageView = nullptr;
};

