#include "VulkanDevice.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"

void VulkanDevice::Init(vk::raii::Instance &instance, GLFWwindow* window)
{
	createSurface(instance, window);
	pickPhysicalDevice(instance);
	createLogicalDevice();
}

void VulkanDevice::createSurface(vk::raii::Instance &instance, GLFWwindow* window)
{
	VkSurfaceKHR _surface;
	std::cout << "Creating window surface..." << std::endl;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
		throw std::runtime_error("failed to create window surface !");
	surface = vk::raii::SurfaceKHR(instance, _surface);
}

bool VulkanDevice::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions =
		std::ranges::all_of(requiredDeviceExtension,
			[&availableDeviceExtensions](auto const& requiredDeviceExtension)
			{
				return std::ranges::any_of(availableDeviceExtensions,
					[requiredDeviceExtension](auto const& availableDeviceExtension)
					{ return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; });
			});

	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void VulkanDevice::pickPhysicalDevice(vk::raii::Instance &instance)
{
	std::cout << "Picking physical device..." << std::endl;
	auto physicalDevices = instance.enumeratePhysicalDevices(); //enumerer les GPU
	if (physicalDevices.empty())
		throw std::runtime_error("failed to find GPUs with Vulkan support !"); //si il y en a pas -> erreur
	auto const bestDevice = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) {
		return isDeviceSuitable(physicalDevice);
		});

	if (bestDevice == physicalDevices.end())
		throw std::runtime_error("failed to find a suitable GPU!");
	physicalDevice = *bestDevice;
}

void VulkanDevice::createLogicalDevice()
{
	std::cout << "Creating logical device..." << std::endl;
	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	auto graphicsQueueFamilyProperty =
		std::ranges::find_if(
			queueFamilyProperties,
			[](auto const& qfp)
			{
				return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) !=
					static_cast<vk::QueueFlags>(0);
			}
		);

	if (graphicsQueueFamilyProperty == queueFamilyProperties.end())
		throw std::runtime_error("Failed to find a graphics queue family!");

	auto graphicsQueueFamilyIndex =
		static_cast<uint32_t>(
			std::distance(
				queueFamilyProperties.begin(),
				graphicsQueueFamilyProperty
			)
			);

	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo
	{
		.queueFamilyIndex = graphicsQueueFamilyIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
				{ .features = { .samplerAnisotropy = true } },
				{ .synchronization2 = true, .dynamicRendering = true },
				{ .extendedDynamicState = true }
	};

	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.enabledExtensionCount =
			static_cast<uint32_t>(requiredDeviceExtension.size()),
		.ppEnabledExtensionNames = requiredDeviceExtension.data()
	};
	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);

	for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
			physicalDevice.getSurfaceSupportKHR(i, *surface))
		{
			queueIndex = i;
			break;
		}
	}
	if (queueIndex == ~0)
		throw std::runtime_error("Could not find a queue for graphics and present");
}
