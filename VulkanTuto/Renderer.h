#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"
#include "VulkanInstance.h"
class Renderer
{
public:
	void Init(GLFWwindow* window);
	void Destroy();
	void createSurface(GLFWwindow* window);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain(GLFWwindow* window);
	void createImageViews();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
private:
	VulkanInstance vulkanInstance;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;

	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	//swap chain variables
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR             swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

};

