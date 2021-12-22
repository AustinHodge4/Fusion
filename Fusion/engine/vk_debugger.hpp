#pragma once
#include <engine/vk_types.hpp>
namespace vk_debug {

#ifdef NDEBUG
	const bool m_enableValidationLayers = false;
#else
	const bool m_enableValidationLayers = true;
#endif

	const std::vector<const char*> m_validationLayers{
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_monitor" // fps
	};

    VkResult create_debug_utils_messenger_ext(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_callback);

    void destroy_debug_utils_messenger_ext(VkInstance p_instance, VkDebugUtilsMessengerEXT p_callback, const VkAllocationCallbacks* p_allocator);

    void populate_debug_messenger_create_info(vk::DebugUtilsMessengerCreateInfoEXT& p_createInfo, vk::Flags<vk::DebugUtilsMessageSeverityFlagBitsEXT> p_messageSeverity, vk::Flags<vk::DebugUtilsMessageTypeFlagBitsEXT> p_messageType, const vk::Bool32& p_debugCallback = 0);

    void setup_debug_messenger(vk::Instance& p_instance, VkDebugUtilsMessengerEXT& p_debugMessenger, vk::Flags<vk::DebugUtilsMessageSeverityFlagBitsEXT> p_messageSeverity, vk::Flags<vk::DebugUtilsMessageTypeFlagBitsEXT> p_messageType);

	bool check_validation_layer_support();

}