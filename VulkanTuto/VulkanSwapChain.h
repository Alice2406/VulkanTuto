#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"

class VulkanSwapChain
{
public :

	void Init(vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device, vk::raii::SurfaceKHR &surface, GLFWwindow* window);
	void Destroy();

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

