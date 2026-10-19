#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"
#include "GLFW/glfw3.h"

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

class Renderer
{
public:
	void Init(GLFWwindow* window);
	void Destroy();

private:
	VulkanInstance vulkanInstance;
	VulkanDevice vulkanDevice;
	VulkanSwapChain vulkanSwapChain;
};

