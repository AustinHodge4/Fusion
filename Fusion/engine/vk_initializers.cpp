#include <engine/vk_initializers.hpp>

vk::CommandPoolCreateInfo vk_init::command_pool_create_info(uint32_t p_queueFamilyIndex, vk::CommandPoolCreateFlagBits p_flags)
{
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.queueFamilyIndex = p_queueFamilyIndex;
    poolInfo.flags = p_flags;
	return poolInfo;
}

vk::CommandBufferAllocateInfo vk_init::command_buffer_allocate_info(vk::CommandPool p_commandPool, uint32_t p_count, vk::CommandBufferLevel p_level)
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = p_level;
    allocInfo.commandPool = p_commandPool;
    allocInfo.commandBufferCount = p_count;
	return allocInfo;
}

vk::CommandBufferBeginInfo vk_init::command_buffer_begin_info(vk::CommandBufferUsageFlagBits p_flags)
{
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = p_flags;
    return beginInfo;
}

VkFramebufferCreateInfo vk_init::framebuffer_create_info(vk::RenderPass p_renderPass, vk::Extent2D p_extent)
{
    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.renderPass = p_renderPass; // renderpass that is compatiable with this framebuffer (same # of attachments and same attachment types)
    framebufferInfo.width = p_extent.width;
    framebufferInfo.height = p_extent.height;
    framebufferInfo.layers = 1; // number of layers in the image view
    return framebufferInfo;
}

vk::FenceCreateInfo vk_init::fence_create_info(vk::FenceCreateFlagBits p_flags)
{
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = p_flags;
    return fenceInfo;
}

vk::SemaphoreCreateInfo vk_init::semaphore_create_info(vk::SemaphoreCreateFlagBits p_flags)
{
    vk::SemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.flags = p_flags;
    return semaphoreInfo;
}

vk::SubmitInfo vk_init::submit_info(vk::CommandBuffer* p_command)
{
    vk::SubmitInfo submitInfo{}; // Setup queue submission
    submitInfo.commandBufferCount = 1; // The command buffers to submit
    submitInfo.pCommandBuffers = p_command;
    return submitInfo;
}

vk::PresentInfoKHR vk_init::present_info()
{
    vk::PresentInfoKHR presentInfo{}; // Present info to submit back to swapchain to show on screen
    presentInfo.pResults = nullptr; // Optional. Get array of vk::Result to check for each swap chain if presentation was successful
    return presentInfo;
}

vk::RenderPassBeginInfo vk_init::renderpass_begin_info(vk::RenderPass p_renderPass, vk::Extent2D p_windowExtent, vk::Framebuffer p_framebuffer)
{
    vk::RenderPassBeginInfo renderPassInfo{}; // begin render pass
    renderPassInfo.renderPass = p_renderPass;
    renderPassInfo.framebuffer = p_framebuffer;
    renderPassInfo.renderArea.offset = vk::Offset2D(0, 0); // render area
    renderPassInfo.renderArea.extent = p_windowExtent;
    return renderPassInfo;
}
