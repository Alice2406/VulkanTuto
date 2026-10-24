#include "VulkanDepth.h"
#define GLFW_INCLUDE_VULKAN

#include "VulkanTextures.h"
#include <iostream>
#include "GLFW/glfw3.h"

void VulkanDepth::Init(vk::raii::PhysicalDevice &physicalDevice, vk::Extent2D swapChainExtent, VulkanTextures &textures, vk::raii::Device &device)
{
	vulkanTextures = &textures;
	createDepthResources(physicalDevice, swapChainExtent, device);
}

void VulkanDepth::createDepthResources(vk::raii::PhysicalDevice &physicalDevice, vk::Extent2D swapChainExtent, vk::raii::Device &device)
{
	std::cout << "Creating depth resources..." << std::endl;
    vk::Format depthFormat = findDepthFormat(physicalDevice);

    std::tie(depthImage, depthImageMemory) = vulkanTextures->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, device, physicalDevice);
    depthImageView = vulkanTextures->createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, device);
}

vk::Format VulkanDepth::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features, vk::raii::PhysicalDevice &physicalDevice)
{
    for (const auto format : candidates) {
        vk::FormatProperties props = physicalDevice.getFormatProperties(format);

        if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
            ((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
            return format;
    }

    throw std::runtime_error("failed to find supported format!");
}

vk::Format VulkanDepth::findDepthFormat(vk::raii::PhysicalDevice &physicalDevice)
{
    return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment, physicalDevice);
}
