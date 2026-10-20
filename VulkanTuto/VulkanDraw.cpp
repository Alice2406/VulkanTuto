#include "VulkanDraw.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "GLFW/glfw3.h"


void VulkanDraw::Init(VulkanDevice& device, VulkanSwapChain& swapChain, VulkanPipeline& pipeline)
{
    vulkanDevice = &device;
    vulkanSwapChain = &swapChain;
    vulkanPipeline = &pipeline;

    createCommandPool();
    createCommandBuffer();
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

void VulkanDraw::createCommandBuffer()
{
	std::cout << "Creating command buffer..." << std::endl;
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	commandBuffer = std::move(vk::raii::CommandBuffers(vulkanDevice->GetDevice(), allocInfo).front());
}

void VulkanDraw::createSyncObjects()
{
    auto& device = vulkanDevice->GetDevice();

    presentCompleteSemaphore =
        vk::raii::Semaphore(device, vk::SemaphoreCreateInfo{});

    drawFence = vk::raii::Fence(
        device,
        { .flags = vk::FenceCreateFlagBits::eSignaled }
    );

    renderFinishedSemaphores.clear();

    const auto& images = vulkanSwapChain->GetSwapChainImages();

    renderFinishedSemaphores.reserve(images.size());

    for (size_t i = 0; i < images.size(); ++i)
    {
        renderFinishedSemaphores.emplace_back(
            device,
            vk::SemaphoreCreateInfo{}
        );
    }
}

void VulkanDraw::drawFrame()
{
    assert(vulkanDevice != nullptr);

    auto& device = vulkanDevice->GetDevice();
    auto& swapChain = vulkanSwapChain->GetSwapChain();
    auto& queue = vulkanDevice->GetQueue();

    auto fenceResult = device.waitForFences(
        *drawFence, vk::True, UINT64_MAX
    );

    if (fenceResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to wait for fence!");
    }

    auto [result, imageIndex] = swapChain.acquireNextImage(
        UINT64_MAX, *presentCompleteSemaphore, nullptr
    );

    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR)
    {
        throw std::runtime_error("failed to acquire swapchain image!");
    }

    auto& renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];

    recordCommandBuffer(imageIndex);

    device.resetFences(*drawFence);

    vk::PipelineStageFlags waitDestinationStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*presentCompleteSemaphore,
        .pWaitDstStageMask = &waitDestinationStageMask,

        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffer,

        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*renderFinishedSemaphore
    };

    queue.submit(submitInfo, *drawFence);

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphore,

        .swapchainCount = 1,
        .pSwapchains = &*swapChain,
        .pImageIndices = &imageIndex
    };

    result = queue.presentKHR(presentInfoKHR);
}

void VulkanDraw::recordCommandBuffer(uint32_t imageIndex)
{
    auto& swapChainImages =
        vulkanSwapChain->GetSwapChainImages();

    auto& swapChainImageViews =
        vulkanSwapChain->GetSwapChainImageViews();

    auto swapChainExtent =
        vulkanSwapChain->GetSwapChainExtent();

    auto& graphicsPipeline =
        vulkanPipeline->GetGraphicsPipeline();

	commandBuffer.begin({});

    transition_image_layout(
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput
    );

    vk::ClearValue clearColor =
        vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderingAttachmentInfo attachmentInfo{
		.imageView = swapChainImageViews[imageIndex], //utilise l'image de swapchain
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal, //layout de l'image
		.loadOp = vk::AttachmentLoadOp::eClear, //efface l'image avant de dessiner
		.storeOp = vk::AttachmentStoreOp::eStore, //stocke l'image
        .clearValue = clearColor
    };

    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo };

    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        *graphicsPipeline
    );

    commandBuffer.setViewport(
        0,
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(swapChainExtent.width),
            static_cast<float>(swapChainExtent.height),
            0.0f,
            1.0f
        )
    );

    commandBuffer.setScissor(
        0,
        vk::Rect2D(
            vk::Offset2D(0, 0),
            swapChainExtent
        )
    );

    commandBuffer.draw(3, 1, 0, 0);

    commandBuffer.endRendering();

    transition_image_layout(
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe
    );

    commandBuffer.end();
}

void VulkanDraw::transition_image_layout(
    uint32_t imageIndex,
    vk::ImageLayout old_layout,
    vk::ImageLayout new_layout,
    vk::AccessFlags2 src_access_mask,
    vk::AccessFlags2 dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask)
{
    auto& swapChainImages =
        vulkanSwapChain->GetSwapChainImages();

    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask = src_stage_mask,
        .srcAccessMask = src_access_mask,
        .dstStageMask = dst_stage_mask,
        .dstAccessMask = dst_access_mask,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapChainImages[imageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vk::DependencyInfo dependency_info = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    commandBuffer.pipelineBarrier2(dependency_info);
}