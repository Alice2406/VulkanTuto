#pragma once
#include "Dependencies.h"

class VulkanSwapChain
{
public :

	void Init(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device, vk::raii::SurfaceKHR &surface, GLFWwindow* window);
	void Destroy();
	vk::SurfaceFormatKHR& GetSurfaceFormat() { return swapChainSurfaceFormat; }
	std::vector<vk::Image>& GetSwapChainImages() { return swapChainImages; }
	std::vector<vk::raii::ImageView>& GetSwapChainImageViews() { return swapChainImageViews; }
	vk::Extent2D GetSwapChainExtent() { return swapChainExtent; }
	vk::raii::SwapchainKHR& GetSwapChain() { return swapChain; }

private :

	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR             swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;

	void createSwapChain(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, GLFWwindow* window);
	void createImageViews(vk::raii::Device& device);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
};

