#include <engine/vk_debugger.hpp>
#include <iostream>
#include <set>

static VKAPI_ATTR vk::Bool32 VKAPI_CALL m_debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT p_messageSeverity,
    vk::DebugUtilsMessageTypeFlagBitsEXT p_messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* p_callbackData,
    void* p_userData) 
{
    std::string_view severity;
    switch (p_messageSeverity) {
    case  vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        severity = "[VERBOSE] ";
        break;
    case  vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        severity = "[INFO] ";
        break;
    case  vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        severity = "[WARN] ";
        break;
    case  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        severity = "[ERROR] ";
        break;
    default:
        severity = "[?] ";
    }
    std::string_view messageType;
    switch (p_messageType) {
    case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
        messageType = "[GENERAL] ";
        break;
    case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
        messageType = "[VALIDATION] ";
        break;
    case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
        messageType = "[PERFOMANCE] ";
        break;
    default:
        messageType = "[?] ";
    }
    std::cerr << severity << messageType << p_callbackData->pMessage << std::endl;

    return false;
}

VkResult vk_debug::create_debug_utils_messenger_ext(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_callback)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(p_instance, p_createInfo, p_allocator, p_callback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vk_debug::destroy_debug_utils_messenger_ext(VkInstance p_instance, VkDebugUtilsMessengerEXT p_callback, const VkAllocationCallbacks* p_allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(p_instance, p_callback, p_allocator);
    }
}

void vk_debug::populate_debug_messenger_create_info(vk::DebugUtilsMessengerCreateInfoEXT& p_createInfo, vk::Flags<vk::DebugUtilsMessageSeverityFlagBitsEXT> p_messageSeverity, vk::Flags<vk::DebugUtilsMessageTypeFlagBitsEXT> p_messageType, const vk::Bool32& p_debugCallback)
{
    p_createInfo.messageSeverity = p_messageSeverity;
    p_createInfo.messageType = p_messageType;
    p_createInfo.pfnUserCallback = p_debugCallback == VK_FALSE ? (PFN_vkDebugUtilsMessengerCallbackEXT)m_debugCallback : (PFN_vkDebugUtilsMessengerCallbackEXT)p_debugCallback;
}

void vk_debug::setup_debug_messenger(vk::Instance& p_instance, VkDebugUtilsMessengerEXT& p_debugMessenger, vk::Flags<vk::DebugUtilsMessageSeverityFlagBitsEXT> p_messageSeverity, vk::Flags<vk::DebugUtilsMessageTypeFlagBitsEXT> p_messageType)
{
    if (!m_enableValidationLayers) return;

    vk::DispatchLoaderDynamic dldi(p_instance, vkGetInstanceProcAddr);

    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    populate_debug_messenger_create_info(createInfo, p_messageSeverity, p_messageType);

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    //debugMessenger = instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);

    // NOTE: reinterpret_cast is also used by vulkan.hpp internally for all these structs
    if (create_debug_utils_messenger_ext(p_instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &p_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug callback!");
    }
}

bool vk_debug::check_validation_layer_support() 
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    std::set<std::string_view> requiredLayers(m_validationLayers.begin(), m_validationLayers.end());

    for (const auto& layer : availableLayers) {
        requiredLayers.erase(layer.layerName);
    }

    return requiredLayers.empty();
}
