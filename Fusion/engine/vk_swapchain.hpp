#pragma once
#include <engine/vk_types.hpp>

namespace vk_swapchain {
    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR m_capabilities;
        std::vector<vk::SurfaceFormatKHR> m_formats;
        std::vector<vk::PresentModeKHR> m_presentModes;
    };

	struct SwapchainInfo {
		vk::PhysicalDevice m_physicalDevice;
		vk::Device m_device;
		vk::SurfaceKHR m_surface;

		std::vector<vk::SurfaceFormatKHR> m_desiredSurfaceFormats;
		vk::SurfaceFormatKHR m_fallbackSurfaceFormat;
		std::vector<vk::PresentModeKHR> m_desiredPresentModes;
		vk::PresentModeKHR m_fallbackPresentMode{ vk::PresentModeKHR::eFifo };
		uint32_t m_desiredWidth{ 0 };
		uint32_t m_desiredHeight{ 0 };
		vk::ImageUsageFlagBits m_imageUsageFlags{ vk::ImageUsageFlagBits::eColorAttachment };
		uint32_t m_graphicsQueueIndex{ 0 };
		uint32_t m_presentQueueIndex{ 0 };
		vk::SurfaceTransformFlagBitsKHR m_preTransform{ vk::SurfaceTransformFlagBitsKHR::eIdentity };
		vk::CompositeAlphaFlagBitsKHR m_compositeAlpha{ vk::CompositeAlphaFlagBitsKHR::eOpaque };
		
		bool m_clipped{ true };
		vk::SwapchainKHR m_oldSwapchain;
	};

    struct SwapchainDetails {
        vk::SwapchainKHR m_swapchain;
        std::vector<vk::Image> m_swapchainImages;
        vk::Format m_swapchainImageFormat;
        vk::Extent2D m_swapchainExtent;
    };

    vk_swapchain::SwapchainSupportDetails query_swapchain_support(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface);

	vk_swapchain::SwapchainDetails create_swapchain(vk_swapchain::SwapchainInfo& p_info);

	std::vector<vk::ImageView> create_image_views(vk::Device& p_device, vk_swapchain::SwapchainDetails p_details);
	vk::ImageView create_image_view(vk::Device& p_device, vk::Image p_image, vk::Format p_format, vk::ImageAspectFlagBits p_aspectFlags, uint32_t p_mipLevels);

}