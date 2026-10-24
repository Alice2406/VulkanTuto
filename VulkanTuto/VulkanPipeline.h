#pragma once
#include "Dependencies.h"

class VulkanDepth;

class VulkanPipeline
{
public :
	void Init(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat, vk::raii::PhysicalDevice physicalDevice, VulkanDepth& depth);
	void Destroy();
	vk::raii::Pipeline& GetGraphicsPipeline() { return graphicsPipeline; }
	vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() { return descriptorSetLayout; }
	vk::raii::PipelineLayout& GetPipelineLayout() { return pipelineLayout; }
private:
	VulkanDepth* vulkanDepth = nullptr;
	vk::PipelineVertexInputStateCreateInfo   vertexInputInfo;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	void createGraphicsPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat, vk::raii::PhysicalDevice physicalDevice);
	void createDescriptorSetLayout(vk::raii::Device& device);
};

