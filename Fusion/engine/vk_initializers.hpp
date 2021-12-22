#pragma once

#include <engine/vk_types.hpp>

namespace vk_init {
	vk::CommandPoolCreateInfo command_pool_create_info(uint32_t p_queueFamilyIndex, vk::CommandPoolCreateFlagBits p_flags = {});

	vk::CommandBufferAllocateInfo command_buffer_allocate_info(vk::CommandPool p_pool, uint32_t p_count = 1, vk::CommandBufferLevel p_level = vk::CommandBufferLevel::ePrimary);

	vk::CommandBufferBeginInfo command_buffer_begin_info(vk::CommandBufferUsageFlagBits p_flags = {});

	VkFramebufferCreateInfo framebuffer_create_info(vk::RenderPass p_renderPass, vk::Extent2D p_extent);

	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits p_flags = {});

	vk::SemaphoreCreateInfo semaphore_create_info(vk::SemaphoreCreateFlagBits p_flags = {});

	vk::SubmitInfo submit_info(vk::CommandBuffer* p_command);

	vk::PresentInfoKHR present_info();

	vk::RenderPassBeginInfo renderpass_begin_info(vk::RenderPass p_renderPass, vk::Extent2D p_windowExtent, vk::Framebuffer p_framebuffer);
}