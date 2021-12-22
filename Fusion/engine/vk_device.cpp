#include <engine/vk_device.hpp>
#include <engine/vk_swapchain.hpp>
#include <engine/vk_debugger.hpp>
#include <map>
#include <set>

vk_device::QueueFamilyIndices vk_device::find_queue_families(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface) 
{
    vk_device::QueueFamilyIndices indices{};

    std::vector<vk::QueueFamilyProperties> queueFamilies = p_physicalDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        vk::Bool32 presentSupport = p_physicalDevice.getSurfaceSupportKHR(i, p_surface);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

vk::UniqueDevice vk_device::create_logical_device(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface, vk_device::QueueFamilyIndices& p_queueFamilies, vk::PhysicalDeviceFeatures& p_deviceFeatures, std::vector<const char*> p_deviceExtensions)
{
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { p_queueFamilies.graphicsFamily, p_queueFamilies.presentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::DeviceCreateInfo createInfo{};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &p_deviceFeatures;

    // for older vulkan versions. Instance and device layers are the same now
    createInfo.enabledExtensionCount = static_cast<uint32_t>(p_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = p_deviceExtensions.data();

    if (vk_debug::m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vk_debug::m_validationLayers.size());
        createInfo.ppEnabledLayerNames = vk_debug::m_validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    vk::UniqueDevice device{};
    try {
        device = p_physicalDevice.createDeviceUnique(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create logical device!");
    }

    //graphicsQueue = device->getQueue(indices.graphicsFamily, 0);
    //presentQueue = device->getQueue(indices.presentFamily, 0);
    return device;
}

vk_device::PhysicalDeviceSelector::PhysicalDeviceSelector(vk::Instance& p_instance, vk::SurfaceKHR& p_surface)
{
    m_instance = p_instance;
    m_surface = p_surface;
}

vk::PhysicalDevice vk_device::PhysicalDeviceSelector::select()
{
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();

    if (devices.size() == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::multimap<int, vk::PhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = rate_device_suitability(device);
        candidates.insert(std::make_pair(score, device));
    }
    
    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        return candidates.rbegin()->second;
        //msaaSamples = getMaxUsableSampleCount();
    }
    else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void vk_device::PhysicalDeviceSelector::add_required_extension(const char* p_extension)
{
    m_deviceExtensions.push_back(p_extension);
}

void vk_device::PhysicalDeviceSelector::add_required_extensions(std::vector<const char*> p_extensions)
{
    m_deviceExtensions.insert(m_deviceExtensions.end(), p_extensions.begin(), p_extensions.end());
}

void vk_device::PhysicalDeviceSelector::set_required_features(vk::PhysicalDeviceFeatures p_deviceFeatures)
{
    m_requiredDeviceFeatures = p_deviceFeatures;
}

int vk_device::PhysicalDeviceSelector::rate_device_suitability(vk::PhysicalDevice p_device) 
{
    vk::PhysicalDeviceProperties deviceProperties;
    deviceProperties = p_device.getProperties();

    vk::PhysicalDeviceFeatures deviceFeatures;
    deviceFeatures = p_device.getFeatures();

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == m_preferredGPUType) {
        score += 1000;
    }
    else if (m_allowAnyGPUType == false) {
        return 0;
    }
    else {
        score += 500;
    }

    if (!supports_features(deviceFeatures, m_requiredDeviceFeatures)) {
        return 0;
    }

    vk_device::QueueFamilyIndices indices = find_queue_families(p_device, m_surface);
    bool extensionsSupported = check_device_extension_support(p_device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        vk_swapchain::SwapchainSupportDetails swapChainSupport = vk_swapchain::query_swapchain_support(p_device, m_surface);
        swapChainAdequate = !swapChainSupport.m_formats.empty() && !swapChainSupport.m_presentModes.empty();
    }

    if (!indices.isComplete() || !extensionsSupported || !swapChainAdequate) return 0;

    return score;
}

bool vk_device::PhysicalDeviceSelector::check_device_extension_support(vk::PhysicalDevice& p_device) 
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, nullptr);

    std::vector<vk::ExtensionProperties> availableExtensions = p_device.enumerateDeviceExtensionProperties();

    std::set<std::string_view> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool vk_device::PhysicalDeviceSelector::supports_features(const vk::PhysicalDeviceFeatures p_supported, const vk::PhysicalDeviceFeatures p_requested) 
{
    //int score = 0;
    if (p_requested.robustBufferAccess && !p_supported.robustBufferAccess) return false;
    if (p_requested.fullDrawIndexUint32 && !p_supported.fullDrawIndexUint32) return false;
    if (p_requested.imageCubeArray && !p_supported.imageCubeArray) return false;
    if (p_requested.independentBlend && !p_supported.independentBlend) return false;
    if (p_requested.geometryShader && !p_supported.geometryShader) return false;
    if (p_requested.tessellationShader && !p_supported.tessellationShader) return false;
    if (p_requested.sampleRateShading && !p_supported.sampleRateShading) return false;
    if (p_requested.dualSrcBlend && !p_supported.dualSrcBlend) return false;
    if (p_requested.logicOp && !p_supported.logicOp) return false;
    if (p_requested.multiDrawIndirect && !p_supported.multiDrawIndirect) return false;
    if (p_requested.drawIndirectFirstInstance && !p_supported.drawIndirectFirstInstance) return false;
    if (p_requested.depthClamp && !p_supported.depthClamp) return false;
    if (p_requested.depthBiasClamp && !p_supported.depthBiasClamp) return false;
    if (p_requested.fillModeNonSolid && !p_supported.fillModeNonSolid) return false;
    if (p_requested.depthBounds && !p_supported.depthBounds) return false;
    if (p_requested.wideLines && !p_supported.wideLines) return false;
    if (p_requested.largePoints && !p_supported.largePoints) return false;
    if (p_requested.alphaToOne && !p_supported.alphaToOne) return false;
    if (p_requested.multiViewport && !p_supported.multiViewport) return false;
    if (p_requested.samplerAnisotropy && !p_supported.samplerAnisotropy) return false;
    if (p_requested.textureCompressionETC2 && !p_supported.textureCompressionETC2) return false;
    if (p_requested.textureCompressionASTC_LDR && !p_supported.textureCompressionASTC_LDR) return false;
    if (p_requested.textureCompressionBC && !p_supported.textureCompressionBC) return false;
    if (p_requested.occlusionQueryPrecise && !p_supported.occlusionQueryPrecise) return false;
    if (p_requested.pipelineStatisticsQuery && !p_supported.pipelineStatisticsQuery) return false;
    if (p_requested.vertexPipelineStoresAndAtomics && !p_supported.vertexPipelineStoresAndAtomics) return false;
    if (p_requested.fragmentStoresAndAtomics && !p_supported.fragmentStoresAndAtomics) return false;
    if (p_requested.shaderTessellationAndGeometryPointSize && !p_supported.shaderTessellationAndGeometryPointSize) return false;
    if (p_requested.shaderImageGatherExtended && !p_supported.shaderImageGatherExtended) return false;
    if (p_requested.shaderStorageImageExtendedFormats && !p_supported.shaderStorageImageExtendedFormats) return false;
    if (p_requested.shaderStorageImageMultisample && !p_supported.shaderStorageImageMultisample) return false;
    if (p_requested.shaderStorageImageReadWithoutFormat && !p_supported.shaderStorageImageReadWithoutFormat) return false;
    if (p_requested.shaderStorageImageWriteWithoutFormat && !p_supported.shaderStorageImageWriteWithoutFormat) return false;
    if (p_requested.shaderUniformBufferArrayDynamicIndexing && !p_supported.shaderUniformBufferArrayDynamicIndexing) return false;
    if (p_requested.shaderSampledImageArrayDynamicIndexing && !p_supported.shaderSampledImageArrayDynamicIndexing) return false;
    if (p_requested.shaderStorageBufferArrayDynamicIndexing && !p_supported.shaderStorageBufferArrayDynamicIndexing) return false;
    if (p_requested.shaderStorageImageArrayDynamicIndexing && !p_supported.shaderStorageImageArrayDynamicIndexing) return false;
    if (p_requested.shaderClipDistance && !p_supported.shaderClipDistance) return false;
    if (p_requested.shaderCullDistance && !p_supported.shaderCullDistance) return false;
    if (p_requested.shaderFloat64 && !p_supported.shaderFloat64) return false;
    if (p_requested.shaderInt64 && !p_supported.shaderInt64) return false;
    if (p_requested.shaderInt16 && !p_supported.shaderInt16) return false;
    if (p_requested.shaderResourceResidency && !p_supported.shaderResourceResidency) return false;
    if (p_requested.shaderResourceMinLod && !p_supported.shaderResourceMinLod) return false;
    if (p_requested.sparseBinding && !p_supported.sparseBinding) return false;
    if (p_requested.sparseResidencyBuffer && !p_supported.sparseResidencyBuffer) return false;
    if (p_requested.sparseResidencyImage2D && !p_supported.sparseResidencyImage2D) return false;
    if (p_requested.sparseResidencyImage3D && !p_supported.sparseResidencyImage3D) return false;
    if (p_requested.sparseResidency2Samples && !p_supported.sparseResidency2Samples) return false;
    if (p_requested.sparseResidency4Samples && !p_supported.sparseResidency4Samples) return false;
    if (p_requested.sparseResidency8Samples && !p_supported.sparseResidency8Samples) return false;
    if (p_requested.sparseResidency16Samples && !p_supported.sparseResidency16Samples) return false;
    if (p_requested.sparseResidencyAliased && !p_supported.sparseResidencyAliased) return false;
    if (p_requested.variableMultisampleRate && !p_supported.variableMultisampleRate) return false;
    if (p_requested.inheritedQueries && !p_supported.inheritedQueries) return false;

    return true;
}
