#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Dependencies.h"

class VulkanVertex;

class VulkanTextures
{
public:
	void Init(VulkanVertex& vertex, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);
	vk::raii::ImageView& getTextureImageView() { return textureImageView; }
	vk::raii::Sampler& getTextureSampler() { return textureSampler; }
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::raii::Device& device);
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice);
private :
	void createTextureImage(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);
	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);
	void createTextureImageView(vk::raii::Device& device);
	void createImageViews(std::vector<vk::raii::ImageView>& swapChainImageViews, std::vector<vk::Image>& swapChainImages, vk::SurfaceFormatKHR& swapChainSurfaceFormat, vk::raii::Device& device);
	void createTextureSampler(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device);

	VulkanVertex* vulkanVertex = nullptr;
	vk::raii::ImageView textureImageView = nullptr;
	vk::raii::Sampler      textureSampler = nullptr;
	vk::raii::Image        textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
};

