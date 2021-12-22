#pragma once

#include <engine/vk_types.hpp>
#include <engine/vk_device.hpp>

class VulkanEngine {
public:

	bool m_isInitialized{ false };
	int m_frameNumber{ 0 };
	double m_deltaTime{ 0 };
	int m_selectedShader{ 0 };

	vk::Extent2D m_windowExtent{ 1700 , 900 };

	struct GLFWwindow* m_window; // forward declare window
	bool m_keys[1024];

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

	// Unique handles destroys itself if it goes out of scope
	vk::UniqueInstance m_instance;
	//vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	// destroyed with vkInstance
	vk::PhysicalDevice m_physicalDevice;
	vk::UniqueDevice m_device;

	// destroyed with vkDevice is destroyed
	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;
	vk_device::QueueFamilyIndices m_queueFamilies;

	vk::SurfaceKHR m_surface;

	vk::SwapchainKHR m_swapchain;
	vk::Format m_swapchainImageFormat;
	vk::Extent2D m_swapchainExtent;
	// destroyed with swapchain
	std::vector<vk::Image> m_swapchainImages;
	std::vector<vk::ImageView> m_swapchainImageViews;
	std::vector<vk::Framebuffer> m_swapchainFramebuffers;

	vk::CommandPool m_commandPool;
	vk::CommandBuffer m_mainCommandBuffer;

	vk::RenderPass m_renderPass;

	vk::Semaphore m_presentSemaphore;
	vk::Semaphore m_renderSemaphore;
	vk::Fence m_renderFence;

	vk::PipelineLayout m_pipelineLayout;
	vk::Pipeline m_graphicsPipeline;
	vk::Pipeline m_colorGraphicsPipeline;
	

private:
	double m_lastTime{ 0 };

	const std::vector<const char*> m_deviceExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	void init_vulkan();

	void init_swapchain();

	void init_commands();

	void init_default_renderpass();

	void init_framebuffers();

	void init_sync_structures();

	vk::UniqueShaderModule load_shader_module(std::string_view p_fileName, bool p_optimize);

	void init_pipelines();

	bool validate_extensions(const char** p_reqExtensions, const uint32_t* p_reqCount, std::vector<const char*>* p_availableExtensions);
	std::vector<const char*> get_required_extensions();
};