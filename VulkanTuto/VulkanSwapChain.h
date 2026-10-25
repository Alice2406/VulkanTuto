#pragma once
#include "Dependencies.h"

class VulkanDepth;

class VulkanSwapChain
{
public :

	void Init(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device, vk::raii::SurfaceKHR &surface, GLFWwindow* window);
	void recreateSwapChain(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, VulkanDepth& depth, GLFWwindow* window);
	vk::SurfaceFormatKHR& GetSurfaceFormat() { return swapChainSurfaceFormat; }
	std::vector<vk::Image>& GetSwapChainImages() { return swapChainImages; }
	std::vector<vk::raii::ImageView>& GetSwapChainImageViews() { return swapChainImageViews; }
	vk::Extent2D GetSwapChainExtent() { return swapChainExtent; }
	vk::raii::SwapchainKHR& GetSwapChain() { return swapChain; }

private :

	VulkanDepth* vulkanDepth = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR             swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;

	void cleanupSwapChain();
	void createSwapChain(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, GLFWwindow* window);
	void createImageViews(vk::raii::Device& device);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

