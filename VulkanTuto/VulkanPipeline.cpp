#include "VulkanPipeline.h"
#include "VulkanDepth.h"
#include "Vertex.h"

#include <fstream>
#include <iostream>

void VulkanPipeline::Init(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat, vk::raii::PhysicalDevice physicalDevice, VulkanDepth& depth)
{
	vulkanDepth = &depth;
	createDescriptorSetLayout(device);
	createGraphicsPipeline(device, surfaceFormat, physicalDevice);
}

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
	std::vector<char> buffer(file.tellg());
	file.seekg(0);
	file.read(buffer.data(), buffer.size());
	file.close();
	return buffer;
}

vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, vk::raii::Device& device)
{
	vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
	return vk::raii::ShaderModule(device, createInfo);
}

void VulkanPipeline::createGraphicsPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat, vk::raii::PhysicalDevice physicalDevice)
{
	std::cout << "Creating Graphics Pipeline..." << std::endl;
	vk::raii::ShaderModule shaderModule = createShaderModule(readFile("../shaders/slang.spv"), device);
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ 
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = shaderModule,  
		.pName = "vertMain" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ 
		.stage = vk::ShaderStageFlagBits::eFragment, 
		.module = shaderModule, 
		.pName = "fragMain" };
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	auto                                     bindingDescription = Vertex::getBindingDescription();
	auto                                     attributeDescriptions = Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo   vertexInputInfo{ .vertexBindingDescriptionCount = 1,
															 .pVertexBindingDescriptions = &bindingDescription,
															 .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
															 .pVertexAttributeDescriptions = attributeDescriptions.data() };

	std::vector<vk::DynamicState>      dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
	vk::PipelineViewportStateCreateInfo      viewportState{ .viewportCount = 1, .scissorCount = 1 };

	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False,
														.rasterizerDiscardEnable = vk::False, 
														.polygonMode = vk::PolygonMode::eFill,
														.cullMode = vk::CullModeFlagBits::eBack,
														.frontFace = vk::FrontFace::eCounterClockwise,
														.depthBiasEnable = vk::False,
														.lineWidth = 1.0f };

	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

	vk::PipelineDepthStencilStateCreateInfo depthStencil{
	.depthTestEnable = vk::True,
	.depthWriteEnable = vk::True,
	.depthCompareOp = vk::CompareOp::eLess,
	.depthBoundsTestEnable = vk::False,
	.stencilTestEnable = vk::False };

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment };

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {
	.setLayoutCount = 1,
	.pSetLayouts = &*descriptorSetLayout,
	.pushConstantRangeCount = 0
	};
	pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &surfaceFormat.format };

	vk::Format depthFormat = vulkanDepth->findDepthFormat(physicalDevice);

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
	{ .stageCount = 2,
	 .pStages = shaderStages,
	 .pVertexInputState = &vertexInputInfo,
	 .pInputAssemblyState = &inputAssembly,
	 .pViewportState = &viewportState,
	 .pRasterizationState = &rasterizer,
	 .pMultisampleState = &multisampling,
	 .pDepthStencilState = &depthStencil,
	 .pColorBlendState = &colorBlending,
	 .pDynamicState = &dynamicState,
	 .layout = pipelineLayout,
	 .renderPass = nullptr },
	{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &surfaceFormat.format, .depthAttachmentFormat = depthFormat } };

	graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

void VulkanPipeline::createDescriptorSetLayout(vk::raii::Device& device)
{
	std::cout << "Creating Descriptor Set Layout..." << std::endl;
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
		vk::DescriptorSetLayoutBinding{
			.binding = 0,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eVertex
		},
		vk::DescriptorSetLayoutBinding{
			.binding = 1,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eFragment
		}
	};
	vk::DescriptorSetLayoutCreateInfo layoutInfo = {
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};
	descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);
}