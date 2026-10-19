#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"

class VulkanDevice
{
public:
	void Init(vk::raii::Instance &instance, GLFWwindow* window);
	void Destroy();
	vk::raii::PhysicalDevice& GetPhysicalDevice() { return physicalDevice; }
	vk::raii::Device& GetDevice() { return device; }
	vk::raii::SurfaceKHR& GetSurface() { return surface; }

private:

	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	void createSurface(vk::raii::Instance &instance, GLFWwindow* window);
	void pickPhysicalDevice(vk::raii::Instance &instance);
	void createLogicalDevice();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
};

