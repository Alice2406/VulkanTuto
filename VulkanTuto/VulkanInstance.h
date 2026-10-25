#pragma once
#include "Dependencies.h"

class VulkanInstance
{
public:

	void CreateInstance();
	vk::raii::Instance& GetInstance() { return instance; }

private:

	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

	std::vector<const char*> GetRequiredExtensions();
	std::vector<const char*> GetRequiredLayers();
	void SetupDebugCallback();
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};

