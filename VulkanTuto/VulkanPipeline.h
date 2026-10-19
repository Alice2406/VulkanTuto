#pragma once
#include "Dependencies.h"

class VulkanPipeline
{
public :
	void Init(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat);
	void Destroy();
private:
	vk::PipelineVertexInputStateCreateInfo   vertexInputInfo;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	void createGraphicsPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat);
};

