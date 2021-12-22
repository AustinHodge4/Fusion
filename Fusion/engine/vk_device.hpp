#pragma once
#include <engine/vk_types.hpp>
#include<vector>

namespace vk_device {
    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;

        bool isComplete() {
            return graphicsFamily >= 0 && presentFamily >= 0;
        }
    };

    QueueFamilyIndices find_queue_families(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface);

    vk::UniqueDevice create_logical_device(vk::PhysicalDevice& p_physicalDevice, vk::SurfaceKHR& p_surface, QueueFamilyIndices& p_queueFamilies, vk::PhysicalDeviceFeatures& p_deviceFeatures, ::std::vector<const char*> p_deviceExtensions);

    class PhysicalDeviceSelector {

    public:
        
        PhysicalDeviceSelector(vk::Instance& p_instance, vk::SurfaceKHR& p_surface);

        vk::PhysicalDeviceType m_preferredGPUType{ vk::PhysicalDeviceType::eDiscreteGpu };
        bool m_allowAnyGPUType{ true };

        vk::PhysicalDevice select();

        void add_required_extension(const char* p_extension);
        // Require a physical device which supports a set of extensions.
        void add_required_extensions(::std::vector<const char*> p_extensions);

        void set_required_features(vk::PhysicalDeviceFeatures p_deviceFeatures);

        int rate_device_suitability(vk::PhysicalDevice p_device);

        bool check_device_extension_support(vk::PhysicalDevice& p_device);

        bool supports_features(const vk::PhysicalDeviceFeatures p_supported, const vk::PhysicalDeviceFeatures p_requested);

    private:
        vk::Instance m_instance;
        vk::SurfaceKHR m_surface;
        ::std::vector<vk::PhysicalDevice> m_physicalDevices;

        ::std::vector<const char*> m_deviceExtensions;
        vk::PhysicalDeviceFeatures m_requiredDeviceFeatures;
    };
}