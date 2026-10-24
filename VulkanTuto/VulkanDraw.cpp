#include "VulkanDraw.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanVertex.h"
#include "VulkanDescriptor.h"
#include "VulkanDepth.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

void VulkanDraw::Init(VulkanDevice& device, VulkanSwapChain& swapChain, VulkanPipeline& pipeline, VulkanVertex& vertex, VulkanDescriptor& descriptor, VulkanDepth& depth, GLFWwindow* window)
{
    vulkanDevice = &device;
    vulkanSwapChain = &swapChain;
    vulkanPipeline = &pipeline;
    vulkanVertex = &vertex;
	vulkanDescriptor = &descriptor;
	vulkanDepth = &depth;

    createCommandPool();
}

void VulkanDraw::InitCommandBuffers()
{
    createCommandBuffers();
    createSyncObjects();
}

void VulkanDraw::createCommandPool()
{
    std::cout << "Creating command pool..." << std::endl;
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = vulkanDevice->GetQueueIndex()
	};
	commandPool = vk::raii::CommandPool(vulkanDevice->GetDevice(), poolInfo);

}

void VulkanDraw::createCommandBuffers()
{
	std::cout << "Creating command buffer..." << std::endl;
    auto& device = vulkanDevice->GetDevice();
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT
	};
	commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
}

void VulkanDraw::createSyncObjects()
{
    auto& device = vulkanDevice->GetDevice();
    const auto& images = vulkanSwapChain->GetSwapChainImages();

    assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());
    for (size_t i = 0; i < images.size(); i++)
        renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
        inFlightFences.emplace_back(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
    }
}

void VulkanDraw::drawFrame(GLFWwindow* window)
{
    auto& device = vulkanDevice->GetDevice();
    auto& swapChain = vulkanSwapChain->GetSwapChain();
    auto& queue = vulkanDevice->GetQueue();

    auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);

    if (fenceResult != vk::Result::eSuccess)
        throw std::runtime_error("failed to wait for fence!");

    device.resetFences(*inFlightFences[frameIndex]);

    auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        vulkanSwapChain->recreateSwapChain(vulkanDevice->GetPhysicalDevice(), device, vulkanDevice->GetSurface(), *vulkanDepth, window);
        return;
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
        assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(frameIndex);
    commandBuffers[frameIndex].reset();
    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
    };

    queue.submit(submitInfo, *inFlightFences[frameIndex]);

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*swapChain,
        .pImageIndices = &imageIndex
    };

    vk::Result presentResult;
    try
    {
        presentResult = queue.presentKHR(presentInfoKHR);
    }
    catch (const vk::OutOfDateKHRError&)
    {
        presentResult = vk::Result::eErrorOutOfDateKHR;
    }

    if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR || framebufferResized)
    {
        framebufferResized = false;
        vulkanSwapChain->recreateSwapChain(vulkanDevice->GetPhysicalDevice(), device, vulkanDevice->GetSurface(), *vulkanDepth, window);
    }
    else if (presentResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanDraw::recordCommandBuffer(uint32_t imageIndex)
{
    auto& swapChainImages = vulkanSwapChain->GetSwapChainImages();
    auto& swapChainImageViews = vulkanSwapChain->GetSwapChainImageViews();
    auto swapChainExtent = vulkanSwapChain->GetSwapChainExtent();
    auto& graphicsPipeline = vulkanPipeline->GetGraphicsPipeline();
	auto& pipelineLayout = vulkanPipeline->GetPipelineLayout();
	auto& depthImage = vulkanDepth->GetDepthImage();
    auto& commandBuffer = commandBuffers[frameIndex];

	commandBuffer.begin({});

    transition_image_layout(
        swapChainImages[imageIndex],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    transition_image_layout(
        *depthImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::ImageAspectFlagBits::eDepth);

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderingAttachmentInfo attachmentInfo{
		.imageView = swapChainImageViews[imageIndex], //utilise l'image de swapchain
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal, //layout de l'image
		.loadOp = vk::AttachmentLoadOp::eClear, //efface l'image avant de dessiner
		.storeOp = vk::AttachmentStoreOp::eStore, //stocke l'image
        .clearValue = clearColor
    };


    vk::RenderingAttachmentInfo depthAttachmentInfo = {
        .imageView = vulkanDepth->GetDepthImageView(),
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .clearValue = clearDepth };

    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo, 
		.pDepthAttachment = &depthAttachmentInfo
    };

    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, static_cast<float>(swapChainExtent.height), static_cast<float>(swapChainExtent.width), -static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));
    commandBuffer.bindVertexBuffers(0, *vulkanVertex->GetVertexBuffer(), { 0 });
    commandBuffer.bindIndexBuffer(*vulkanVertex->GetIndexBuffer(), 0, vk::IndexType::eUint16);
	commandBuffers[frameIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vulkanPipeline->GetPipelineLayout(), 0, *vulkanDescriptor->GetDescriptorSets()[frameIndex], {});
    commandBuffer.drawIndexed(static_cast<uint32_t>(vulkanVertex->GetIndicesCount()), 1, 0, 0, 0);
    commandBuffer.endRendering();

    transition_image_layout(
        swapChainImages[imageIndex],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor
    );

    commandBuffer.end();
}

void VulkanDraw::updateUniformBuffer(uint32_t currentImage)
{
    auto swapChainExtent = vulkanSwapChain->GetSwapChainExtent();
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
	memcpy(vulkanVertex->GetUniformBuffersMapped()[currentImage], &ubo, sizeof(ubo));
}

void VulkanDraw::transition_image_layout(vk::Image image, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask, vk::ImageAspectFlags image_aspect_flags)
{
    vk::ImageMemoryBarrier2 barrier = {
                .srcStageMask = src_stage_mask,
                .srcAccessMask = src_access_mask,
                .dstStageMask = dst_stage_mask,
                .dstAccessMask = dst_access_mask,
                .oldLayout = old_layout,
                .newLayout = new_layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange = {
                    .aspectMask = image_aspect_flags,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1 } };
    vk::DependencyInfo dependency_info = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier };
    commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
}