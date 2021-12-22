#include <engine/vk_swapchain.hpp>
#include <engine/vk_device.hpp>

vk::SurfaceFormatKHR choose_surface_format(const std::vector<vk::SurfaceFormatKHR>& p_availableSurfaceFormats, const vk_swapchain::SwapchainInfo& p_info);
vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& p_availablePresentModes, const vk_swapchain::SwapchainInfo& p_info);
vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& p_capabilities, const vk_swapchain::SwapchainInfo& p_info);

vk_swapchain::SwapchainSupportDetails vk_swapchain::query_swapchain_support(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface) 
{
    vk_swapchain::SwapchainSupportDetails details{};
    details.m_capabilities = p_physicalDevice.getSurfaceCapabilitiesKHR(p_surface);
    details.m_formats = p_physicalDevice.getSurfaceFormatsKHR(p_surface);
    details.m_presentModes = p_physicalDevice.getSurfacePresentModesKHR(p_surface);

    return details;
}

vk_swapchain::SwapchainDetails vk_swapchain::create_swapchain(vk_swapchain::SwapchainInfo& p_info) 
{
    vk_swapchain::SwapchainSupportDetails swapChainSupport = query_swapchain_support(p_info.m_physicalDevice, p_info.m_surface);

    vk::SurfaceFormatKHR surfaceFormat = choose_surface_format(swapChainSupport.m_formats, p_info);
    vk::PresentModeKHR presentMode = choose_present_mode(swapChainSupport.m_presentModes, p_info);
    vk::Extent2D extent = choose_extent(swapChainSupport.m_capabilities, p_info);

    uint32_t imageCount = swapChainSupport.m_capabilities.minImageCount + 1;

    if (swapChainSupport.m_capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_capabilities.maxImageCount) {
        imageCount = swapChainSupport.m_capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = p_info.m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // should be 1 unless building stereoscopic images
    createInfo.imageUsage = p_info.m_imageUsageFlags;
    
    // ^ It is also possible that you'll render images to a separate image first to perform operations like post-processing. 
    // In that case you may use a value like vk::ImageUsageFlagBits::eTransferDst instead and 
    // use a memory operation to transfer the rendered image to a swap chain image.

    //vk_device::QueueFamilyIndices indices = vk_device::find_queue_families(p_info.m_physicalDevice, p_info.m_surface);
    uint32_t queueFamilyIndices[] = { p_info.m_graphicsQueueIndex, p_info.m_presentQueueIndex };

    if (p_info.m_graphicsQueueIndex != p_info.m_presentQueueIndex) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive; // image is owned by one queue family at a time
        //createInfo.queueFamilyIndexCount = 0; // Optional
        //createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform; // disallow screen transform like 90 deg window flip
    createInfo.compositeAlpha = p_info.m_compositeAlpha; // ignore alpha channel blending with other windows
    createInfo.presentMode = presentMode;
    createInfo.clipped = p_info.m_clipped; // ignore hidden pixels ie. from another window in front
    createInfo.oldSwapchain = p_info.m_oldSwapchain; // if the window was resized we need to recreate swapchain in pass in old one

    vk::SwapchainKHR swapchain{};
    try {
        swapchain = p_info.m_device.createSwapchainKHR(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create swap chain!");
    }
    vk_swapchain::SwapchainDetails details{};
    details.m_swapchain = swapchain;
    details.m_swapchainImages = p_info.m_device.getSwapchainImagesKHR(swapchain);
    details.m_swapchainImageFormat = surfaceFormat.format;
    details.m_swapchainExtent = extent;

    return details;
}

vk::SurfaceFormatKHR choose_surface_format(const std::vector<vk::SurfaceFormatKHR>& p_availableSurfaceFormats, const vk_swapchain::SwapchainInfo& p_info) 
{
    // Require sRGB with RGBA 8bit format
    if (p_availableSurfaceFormats.size() == 1 && p_availableSurfaceFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& surfaceFormat : p_availableSurfaceFormats) {
        if (std::find_if(p_info.m_desiredSurfaceFormats.begin(), p_info.m_desiredSurfaceFormats.end(),
            [&surfaceFormat](const vk::SurfaceFormatKHR desired) {
                return surfaceFormat.format == desired.format && surfaceFormat.colorSpace == desired.colorSpace;
            }) != p_info.m_desiredSurfaceFormats.end()) {
            
            return surfaceFormat;
        }
    }
    // fallback
    return p_info.m_fallbackSurfaceFormat.format != vk::Format::eUndefined ? p_info.m_fallbackSurfaceFormat : p_availableSurfaceFormats[0];
}

vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& p_availablePresentModes, const vk_swapchain::SwapchainInfo& p_info) 
{
    // vk::PresentModeKHR::eImmediate: Images submitted by your application are transferred to the screen right away, which may result in tearing.
    // vk::PresentModeKHR::eFifo :If the queue is full then the program has to wait (vsync)
    // vk::PresentModeKHR::eFifoRelaxed : Similar to previous with no waiting. This may result in visible tearing.
    // vk::PresentModeKHR::eMailbox : This is another variation of the second mode.
    //                               Instead of blocking the application when the queue is full, the images that are already queued are simply replaced with the newer ones.
    //                               This mode can be used to implement triple buffering, which allows you to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering.

    for (const auto& presentMode : p_availablePresentModes) {
        if (std::find_if(p_info.m_desiredPresentModes.begin(), p_info.m_desiredPresentModes.end(),
            [&presentMode](const vk::PresentModeKHR desired) { return presentMode == desired;}) != p_info.m_desiredPresentModes.end()) {

            return presentMode;
        }
    }
    // fallback
    return p_info.m_fallbackPresentMode;
}

vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& p_capabilities, const vk_swapchain::SwapchainInfo& p_info) 
{
    // the resolution of the swap chain images
    // Cant use WIDTH / HEIGHT since it's in screen coords and high DPI monitors screen coords do not match pixels exactly
    // currentExtent is in pixels
    if (p_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return p_capabilities.currentExtent;
    }
    else {
        vk::Extent2D actualExtent = { p_info.m_desiredWidth, p_info.m_desiredHeight };

        actualExtent.width = std::max(p_capabilities.minImageExtent.width, std::min(p_capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(p_capabilities.minImageExtent.height, std::min(p_capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

std::vector<vk::ImageView> vk_swapchain::create_image_views(vk::Device& p_device, vk_swapchain::SwapchainDetails p_details) 
{
    // describes how to access the image and which part of the image to access
    std::vector<vk::ImageView> swapchainImageViews{ p_details.m_swapchainImages.size() };
    for (size_t i = 0; i < p_details.m_swapchainImages.size(); i++) {
        swapchainImageViews[i] = create_image_view(p_device, p_details.m_swapchainImages[i], p_details.m_swapchainImageFormat, vk::ImageAspectFlagBits::eColor, 1);
    }
    return swapchainImageViews;
}

vk::ImageView vk_swapchain::create_image_view(vk::Device& p_device, vk::Image p_image, vk::Format p_format, vk::ImageAspectFlagBits p_aspectFlags, uint32_t p_mipLevels) 
{
    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.image = p_image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = p_format;
    // map channels to default
    viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.a = vk::ComponentSwizzle::eIdentity;
    // image purpose; color with no mipmaps or multiple layers
    // (stereographic) You could then create multiple image views for each image representing the views for the left and right eyes by accessing different layers.
    viewInfo.subresourceRange.aspectMask = p_aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = p_mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::ImageView imageView;
    try {
        imageView = p_device.createImageView(viewInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}
