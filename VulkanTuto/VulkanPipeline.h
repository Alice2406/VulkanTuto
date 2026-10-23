#pragma once
#include "Dependencies.h"

class VulkanPipeline
{
public :
	void Init(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat);
	void Destroy();
	vk::raii::Pipeline& GetGraphicsPipeline() { return graphicsPipeline; }
	vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() { return descriptorSetLayout; }
	vk::raii::PipelineLayout& GetPipelineLayout() { return pipelineLayout; }
private:
	vk::PipelineVertexInputStateCreateInfo   vertexInputInfo;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	void createGraphicsPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat);
	void createDescriptorSetLayout(vk::raii::Device& device);
};

