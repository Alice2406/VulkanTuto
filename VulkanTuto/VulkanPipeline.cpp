#include "VulkanPipeline.h"
#include <fstream>
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"
#include "Vertex.h"

void VulkanPipeline::Init(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat)
{
	createDescriptorSetLayout(device);
	createGraphicsPipeline(device, surfaceFormat);
}

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary); //ouvre le fichier en mode binaire et commence la lecture à la fin du fichier

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
	std::vector<char> buffer(file.tellg()); //allouer l'espace nécessaire pour le buffer en utilisant la taille du fichier (ce qui explique pourquoi on est a la fin du fichier)
	file.seekg(0); //on se remet au debut du fichier
	file.read(buffer.data(), buffer.size());
	file.close();
	return buffer;
}

vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, vk::raii::Device& device)
{
	vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) }; //reinterpret_cast pour convertir le pointeur de char en pointeur de uint32_t
	return vk::raii::ShaderModule(device, createInfo);
}
void VulkanPipeline::createGraphicsPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat)
{
	std::cout << "Creating Graphics Pipeline..." << std::endl;
	vk::raii::ShaderModule shaderModule = createShaderModule(readFile("../shaders/slang.spv"), device);
	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ 
		.stage = vk::ShaderStageFlagBits::eVertex,  //stage = type de shader
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

	std::vector<vk::DynamicState>      dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor }; //on va changer le viewport et le scissor à chaque frame
	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
	vk::PipelineViewportStateCreateInfo      viewportState{ .viewportCount = 1, .scissorCount = 1 };

	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False, //on ne veut pas que les fragments soient coupés à la limite du viewport
														.rasterizerDiscardEnable = vk::False, 
														.polygonMode = vk::PolygonMode::eFill,
														.cullMode = vk::CullModeFlagBits::eBack,
														.frontFace = vk::FrontFace::eCounterClockwise, //on va dessiner les triangles dans le sens des aiguilles d'une montre
														.depthBiasEnable = vk::False,
														.lineWidth = 1.0f };

	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False }; //on ne veut pas d'anti-aliasing

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
	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
	{ .stageCount = 2,
	 .pStages = shaderStages,
	 .pVertexInputState = &vertexInputInfo,
	 .pInputAssemblyState = &inputAssembly,
	 .pViewportState = &viewportState,
	 .pRasterizationState = &rasterizer,
	 .pMultisampleState = &multisampling,
	 .pColorBlendState = &colorBlending,
	 .pDynamicState = &dynamicState,
	 .layout = pipelineLayout,
	 .renderPass = nullptr },
	{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &surfaceFormat.format } };

	graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

void VulkanPipeline::createDescriptorSetLayout(vk::raii::Device& device)
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding = {
		.binding = 0,
		.descriptorType = vk::DescriptorType::eUniformBuffer,
		.descriptorCount = 1,
		.stageFlags = vk::ShaderStageFlagBits::eVertex
	};
	vk::DescriptorSetLayoutCreateInfo layoutInfo = {
		.bindingCount = 1,
		.pBindings = &uboLayoutBinding
	};
	descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);

}