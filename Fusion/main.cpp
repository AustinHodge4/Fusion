// #define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <GLFW/glfw3.h>

//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // change -1,1 (opengl) to 0,1 (vulkan)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <Camera.hpp>

#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include <engine/vk_engine.hpp>

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

const std::string MODEL_PATH = "models/viking/viking_room.obj";
const std::string TEXTURE_PATH = "models/viking/textures/viking_room.png";

const int MAX_FRAMES_IN_FLIGHT = 4; // Limit number of framebuffers worked on async

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
    "VK_LAYER_LUNARG_monitor" // fps
};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;

    bool isComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex; // move to next vertex

        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat; // vec3 signed floats
        attributeDescriptions[0].offset = offsetof(Vertex, pos); // basically 0

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat; // vec3 signed floats
        attributeDescriptions[1].offset = offsetof(Vertex, color); // # of bytes to offset from Vertex obj

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat; 
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord); // # of bytes to offset from Vertex obj

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        // hash the vertex struct in order to use it in map
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

// Need to make sure the byte alignment is multiple of the base. https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets
// could enable #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES but it breaks down for nested structs
// vec4/vec3/mat4 must be aligned by 16 bytes ( 1 float32 = 4bytes )
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

//std::vector<Vertex> vertices = {
//    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
//
//    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
//};
//const std::vector<uint16_t> indices = {
//    0, 1, 2, 2, 3, 0,
//    4, 5, 6, 6, 7, 4
//};

class HelloTriangleApplication {
public:
    void run() {
        if (useCamera) {
            initCamera();
        }
        
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    bool keys[1024];
    // Unique handles destroys itself if it goes out of scope
    vk::UniqueInstance instance;
    //vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;
    VkDebugUtilsMessengerEXT debugMessenger;
    // destroyed with vkInstance
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice device;

    // destroyed with vkDevice is destroyed
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::SurfaceKHR  surface;

    vk::SwapchainKHR swapChain;
    // destroyed with swapchain
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::ImageView> swapChainImageViews;
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;

    vk::RenderPass renderPass;

    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    // destroyed with descriptor pool
    std::vector<vk::DescriptorSet> descriptorSets;
    //vk::UniqueDescriptorSetLayout descriptorSetLayout;

    std::array<vk::DynamicState, 3> dynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth,
        vk::DynamicState::eScissor
    };

    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
    vk::UniqueShaderModule vertShaderModule;
    vk::UniqueShaderModule fragShaderModule;

    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> commandBuffers;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    uint32_t mipLevels;
    
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureImageView;
    vk::Sampler textureSampler;

    // create depth image since swapchain doesn't create one for us. Only one since we are drawing one time at a time
    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;

    // create msaa image since we are using multiple samples per pixel
    vk::Image colorImage;
    vk::DeviceMemory colorImageMemory;
    vk::ImageView colorImageView;
    vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;

    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;

    Camera camera;
    double lastMouseX{ 0 }, lastMouseY{ 0 };
    bool useCamera{ true };
    double deltaTime{ 0.0 };
    double lastFrame{ 0.0 };

    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    size_t currentFrame{ 0 };

    bool framebufferResized = false;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this); // store app pointer so we can use it in the callback
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback); // screen resize callback
        glfwSetKeyCallback(window, key_callback);
        if (useCamera) {
            glfwSetCursorPosCallback(window, cursor_position_callback);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetScrollCallback(window, scroll_callback);
        }
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        /*if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            
            vertices[0].pos.x += 0.05f;
            vertices[1].pos.x += 0.05f;
            vertices[2].pos.x += 0.05f;
            app->rebuffer();
        }
        else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {

            vertices[0].pos.x -= 0.05f;
            vertices[1].pos.x -= 0.05f;
            vertices[2].pos.x -= 0.05f;
            app->rebuffer();
        }*/

        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS) {
                app->keys[key] = true;
            }
            else if (action == GLFW_RELEASE) {
                app->keys[key] = false;
            }
                
        }
    }

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->camera.look(xpos - app->lastMouseX, app->lastMouseY - ypos);
        app->lastMouseX = xpos;
        app->lastMouseY = ypos;
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->camera.process_zoom(yoffset);
    }


    void handleInput() {
        if (useCamera) {
            if (keys[GLFW_KEY_UP]) {
                camera.move(GLFW_KEY_UP, deltaTime);
            }
            if (keys[GLFW_KEY_DOWN]) {
                camera.move(GLFW_KEY_DOWN, deltaTime);
            }
            if (keys[GLFW_KEY_RIGHT]) {
                camera.move(GLFW_KEY_RIGHT, deltaTime);
            }
            if (keys[GLFW_KEY_LEFT]) {
                camera.move(GLFW_KEY_LEFT, deltaTime);
            }
        }
    }

    void initCamera() {
        camera = Camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f), 2.0f, 35.0f);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createColorResources();
        createDepthResources();
        createFramebuffers();
        createCommandPool();
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        createSyncObjects();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) { // pause here if the screen is minimized
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        device->waitIdle();

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createRenderPass();
        //createGraphicsPipeline();
        createColorResources();
        createDepthResources();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();

        imagesInFlight.resize(swapChainImages.size(), nullptr);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            auto current = glfwGetTime();
            deltaTime = current - lastFrame;
            lastFrame = current;
            //std::cout << "dt: " << deltaTime << std::endl;

            glfwPollEvents();
            handleInput();
            drawFrame();
        }
        device->waitIdle(); // Wait until device is done with async operations then cleanup
    }

    void cleanupSwapChain() {
        for (auto framebuffer : swapChainFramebuffers) {
            device->destroyFramebuffer(framebuffer);
        }

        device->freeCommandBuffers(commandPool, commandBuffers);

        /*device->destroyPipeline(graphicsPipeline);
        device->destroyPipelineLayout(pipelineLayout);*/
        device->destroyRenderPass(renderPass);

        device->destroyImageView(colorImageView);
        device->destroyImage(colorImage);
        device->freeMemory(colorImageMemory);

        device->destroyImageView(depthImageView);
        device->destroyImage(depthImage);
        device->freeMemory(depthImageMemory);

        for (auto imageView : swapChainImageViews) {
            device->destroyImageView(imageView);
        }
        // not using UniqeSwapchain to destroy in correct order - before the surface
        device->destroySwapchainKHR(swapChain);

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            device->destroyBuffer(uniformBuffers[i]);
            device->freeMemory(uniformBuffersMemory[i]);
        }

        device->destroyDescriptorPool(descriptorPool);
    }

    void cleanup() {

        cleanupSwapChain();

        device->destroyPipeline(graphicsPipeline);
        device->destroyPipelineLayout(pipelineLayout);

        device->destroySampler(textureSampler);
        device->destroyImageView(textureImageView);
        device->destroyImage(textureImage);
        device->freeMemory(textureImageMemory);

        device->destroyDescriptorSetLayout(descriptorSetLayout);

        device->destroyBuffer(vertexBuffer);
        device->freeMemory(vertexBufferMemory);
        device->destroyBuffer(indexBuffer);
        device->freeMemory(indexBufferMemory);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device->destroySemaphore(renderFinishedSemaphores[i]);
            device->destroySemaphore(imageAvailableSemaphores[i]);
            device->destroyFence(inFlightFences[i]);
        }

        device->destroyCommandPool(commandPool);

        // surface is created by glfw, therefore not using a Unique handle
        instance->destroySurfaceKHR(surface);
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(*instance, debugMessenger, nullptr);
            //instance->destroyDebugUtilsMessengerEXT(*debugMessenger);
        }
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        try {
            instance = vk::createInstanceUnique(createInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void createSurface() {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(*instance, window, nullptr, &rawSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        surface = rawSurface;
        //vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(*instance);
        //surface = vk::UniqueSurfaceKHR(rawSurface, _deleter);
    }

    void pickPhysicalDevice() {
        std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();

        if (devices.size() == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::multimap<int, vk::PhysicalDevice> candidates;
        for (const auto& device : devices) {
            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0) {
            physicalDevice = candidates.rbegin()->second;
            msaaSamples = getMaxUsableSampleCount();
        }
        else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Enable device features that are used 
        vk::PhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.fillModeNonSolid = true;
        deviceFeatures.samplerAnisotropy = true;
        deviceFeatures.sampleRateShading = true;
        deviceFeatures.wideLines = true;

        vk::DeviceCreateInfo createInfo{};
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        // for older vulkan versions. Instance and device layers are the same now
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        try {
            device = physicalDevice.createDeviceUnique(createInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create logical device!");
        }

        graphicsQueue = device->getQueue(indices.graphicsFamily, 0);
        presentQueue = device->getQueue(indices.presentFamily, 0);
    }

    int rateDeviceSuitability(vk::PhysicalDevice device) {
        vk::PhysicalDeviceProperties deviceProperties;
        deviceProperties = device.getProperties();

        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures = device.getFeatures();

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }

        // Application can't function without wide lines
        if (!deviceFeatures.wideLines) {
            return 0;
        }

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        // Application can't function without line mode
        if (!deviceFeatures.fillModeNonSolid) {
            return 0;
        }

        // Application can't function without anisotropy mode
        if (!deviceFeatures.samplerAnisotropy) {
            return 0;
        }

        QueueFamilyIndices indices = findQueueFamilies(device);
        bool extensionsSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        if (!indices.isComplete() || !extensionsSupported || !swapChainAdequate) return 0;

        return score;
    }

    bool checkDeviceExtensionSupport(vk::PhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

        std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) {
        QueueFamilyIndices indices{};

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
            }
            vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);

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

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device) {
        SwapChainSupportDetails details{};
        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);

        return details;
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo{};
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // should be 1 unless building stereoscopic images
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        // ^ It is also possible that you'll render images to a separate image first to perform operations like post-processing. 
        // In that case you may use a value like vk::ImageUsageFlagBits::eTransferDst instead and 
        // use a memory operation to transfer the rendered image to a swap chain image.

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive; // image is owned by one queue family at a time
            //createInfo.queueFamilyIndexCount = 0; // Optional
            //createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform; // disallow screen transform like 90 deg window flip
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque; // ignore alpha channel blending with other windows
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE; // ignore hidden pixels ie. from another window in front
        createInfo.oldSwapchain = nullptr; // if the window was resized we need to recreate swapchain in pass in old one

        try {
            swapChain = device->createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create swap chain!");
        }

        swapChainImages = device->getSwapchainImagesKHR(swapChain);
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        // Require sRGB with RGBA 8bit format
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
            return { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }
        // fallback
        return availableFormats[0];
    }

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        // vk::PresentModeKHR::eImmediate: Images submitted by your application are transferred to the screen right away, which may result in tearing.
        // vk::PresentModeKHR::eFifo :If the queue is full then the program has to wait (vsync)
        // vk::PresentModeKHR::eFifoRelaxed : Similar to previous with no waiting. This may result in visible tearing.
        // vk::PresentModeKHR::eMailbox : This is another variation of the second mode.
        //                               Instead of blocking the application when the queue is full, the images that are already queued are simply replaced with the newer ones.
        //                               This mode can be used to implement triple buffering, which allows you to avoid tearing with significantly less latency issues than standard vertical sync that uses double buffering.

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }
        // fallback
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        // the resolution of the swap chain images
        // Cant use WIDTH / HEIGHT since it's in screen coords and high DPI monitors screen coords do not match pixels exactly
        // currentExtent is in pixels
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            vk::Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags, uint32_t mipLevels) {
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = format;
        // map channels to default
        viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
        viewInfo.components.a = vk::ComponentSwizzle::eIdentity;
        // image purpose; color with no mipmaps or multiple layers
        // (stereographic) You could then create multiple image views for each image representing the views for the left and right eyes by accessing different layers.
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView;
        try {
            imageView = device->createImageView(viewInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void createImageViews() {
        // describes how to access the image and which part of the image to access
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, vk::ImageAspectFlagBits::eColor, 1);
        }
    }

    void createRenderPass() {
        // Tell vulkan how we are going to use the framebuffers

        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat; // should match swapchain image format
        colorAttachment.samples = msaaSamples; // multi-samples
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // What to do with color data in attachment before rendering. Clear it before drawing new frame
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // What to do with color data in attachment after rendering. Store it so we can see it
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined; // don't care. Transform image to another layout. Where initial is incoming input and final is output
        colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal; // ready to present (vk::ImageLayout::ePresentSrcKHR) multisampled images need to be resolved to a regular image first

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0; // Refrence index of attachment from pAttachments array (see renderPassInfo). This case is 0 since this is the first in the array
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal; // Layout to transform attachment to during a subpass which has this attachment

        vk::AttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = swapChainImageFormat; // should match swapchain image format
        colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1; // multi-samples
        colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare; // What to do with color data in attachment before rendering. Clear it before drawing new frame
        colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore; // What to do with color data in attachment after rendering. Store it so we can see it
        colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined; // don't care. Transform image to another layout. Where initial is incoming input and final is output
        colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR; // present multi sampled to screen

        vk::AttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2; // Refrence index of attachment from pAttachments array (see renderPassInfo). This case is 0 since this is the first in the array
        colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal; // Layout to transform attachment to during a subpass which has this attachment

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = msaaSamples; // multi-samples
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear; // What to do with depth data in attachment before rendering. Clear it before drawing new frame
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare; // What to do with depth data in attachment after rendering
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined; // Transform image to another layout. Where initial is incoming input and final is output
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1; // Refrence index of attachment from pAttachments array (see renderPassInfo).
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal; // Layout to transform attachment to during a subpass which has this attachment

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics; // Type of subpass (graphics, compute, raytracing)
        subpass.colorAttachmentCount = 1; // The attachments that should go with this subpass
        subpass.pColorAttachments = &colorAttachmentRef; // The index of the attachment array is directly related to layout(location = #) in fragment shader
        subpass.pDepthStencilAttachment = &depthAttachmentRef; 
        subpass.pResolveAttachments = &colorAttachmentResolveRef; // use multi sample resolve method

        vk::SubpassDependency dependency{}; // Have the subpass depend on one. By default there are hidden implicit subpasses before and after our defined subpass
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // refers to the implicit subpass before
        dependency.dstSubpass = 0; // our subpass (index in pSubpasses)
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests; // Wait on the color attachment output from swapchain and before fragments
        //dependency.srcAccessMask = 0;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite; // allow read and write for our subpass

        std::array<vk::AttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
        vk::RenderPassCreateInfo renderPassInfo{}; // create render pass with array of attachments and subpasses
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        try {
            renderPass = device->createRenderPass(renderPassInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void createGraphicsPipeline() {
        //  The graphics pipeline is immutable, so you must recreate the pipeline from scratch if you want to change shaders, bind different framebuffers or change the blend function
        // index/vertex buffer -> input assembler* -> vertex shader -> tessellation shader -> geometry shader -> rasterization* -> fragment shader -> color blending* -> framebuffer
        // shaders written in bytecode (SPIR-V) since some GPU venders are flexible with their interperation of GLSL...

        if (!vertShaderModule.get() || !fragShaderModule.get()) { // Only compile if not found                 
            //auto vertShaderCode = readFile("shaders/vert.spv");
            //auto fragShaderCode = readFile("shaders/frag.spv");

            vertShaderModule = createShaderModule("base.vert", true);
            fragShaderModule = createShaderModule("base.frag", true);
        }

        vk::PipelineShaderStageCreateInfo shaderStages[]{
            {
                vk::PipelineShaderStageCreateFlags(),
                vk::ShaderStageFlagBits::eVertex, // pipeline stage (vertex)
                *vertShaderModule,
                "main" //entry point
            },
            {
                vk::PipelineShaderStageCreateFlags(),
                vk::ShaderStageFlagBits::eFragment,
                *fragShaderModule,
                "main"
            }
        };

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{}; // Describes what kind of geometry to draw
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE; // If true, possible to break up lines and triangles in the _STRIP topology modes by using a special index

        //vk::Viewport viewport = {}; // Describes the section of the window to draw
        //viewport.x = 0.0f;
        //viewport.y = 0.0f;
        //viewport.width = (float)swapChainExtent.width;
        //viewport.height = (float)swapChainExtent.height;
        //viewport.minDepth = 0.0f;
        //viewport.maxDepth = 1.0f;

        //vk::Rect2D scissor = {}; // Describes the section of the viewport to include. Anything outside is discarded by the rasterizer
        //scissor.offset = vk::Offset2D(0, 0);
        //scissor.extent = swapChainExtent;

        vk::PipelineDynamicStateCreateInfo dynamicState{}; // Use dynamic states to change viewport and line width without recreating the pipeling
        dynamicState.dynamicStateCount = 3;
        dynamicState.pDynamicStates = dynamicStates.data();

        vk::PipelineViewportStateCreateInfo viewportState{}; // Combine all the viewports and states since some GPUs can support multiple
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr; // Using dynamic
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr; // Using dynamic

        vk::PipelineRasterizationStateCreateInfo rasterizer{}; // Rasterizer; turns into fragments; does depth test, scissor test and face culling
        rasterizer.depthClampEnable = VK_FALSE; // If true, fragments beyond near and far planes are clamped instead of discarded (req GPU feature)
        rasterizer.rasterizerDiscardEnable = VK_FALSE; // If true, geometry never passes this phase
        rasterizer.polygonMode = vk::PolygonMode::eFill; // How fragments are generated for geometry (LINE and POINT req GPU feature)
        rasterizer.lineWidth = 1.0f; // using dynamics. Thickness of fragments. Anything above 1.0 requires GPU feature (wideLines). Maximum is hardware dependent
        rasterizer.cullMode = vk::CullModeFlagBits::eBack; // Culling
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise; // Vertex order for faces
        rasterizer.depthBiasEnable = VK_FALSE; // Can alter depth values
        //rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        //rasterizer.depthBiasClamp = 0.0f; // Optional
        //rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        vk::PipelineMultisampleStateCreateInfo multisampling{}; // Multisampling for anti-aliasing
        multisampling.sampleShadingEnable = VK_TRUE; // Improves quality further
        multisampling.rasterizationSamples = msaaSamples;
        multisampling.minSampleShading = 0.2f; // Optional closer to 1 is smooth
        //multisampling.pSampleMask = nullptr; // Optional
        //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        //multisampling.alphaToOneEnable = VK_FALSE; // Optional

        
        vk::PipelineDepthStencilStateCreateInfo depthStencil{}; // Depth and stencil buffer
        depthStencil.depthTestEnable = VK_TRUE; // enable depth testing
        depthStencil.depthWriteEnable = VK_TRUE; // write depth values that pass to the buffer
        depthStencil.depthCompareOp = vk::CompareOp::eLess; // keep lower depth values fragments
        depthStencil.depthBoundsTestEnable = VK_FALSE; // Enable to keep fragments that fall within a range
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE; // Enable stencil testing
        depthStencil.front = vk::StencilOp::eZero; // Optional
        depthStencil.back = vk::StencilOp::eZero; // Optional

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{}; // Color blending (make one for each framebuffer)
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        //colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional // determine the rgb and a blending ie. rgb = (sf * c) op (df * dc) & colorWriteMask
        //colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
        //colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
        //colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
        //colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
        //colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.logicOpEnable = VK_FALSE; // set to true to use second way of blending instead of the options above. This will automatically set blend to false for every framebuffer
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{}; // Pass any uniforms to shaders
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Tell the pipeline we are using descriptor set layouts for this shader
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        try {
            pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo{}; // Build pipeline info
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState; // Optional
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr; // a base pipeline to derive from

        try {
            graphicsPipeline = device->createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }

    vk::UniqueShaderModule createShaderModule(const std::string_view& fileName, bool optimize) {
        const std::string fileLocation = "shaders/" + std::string(fileName.data());

        // Check if file exists
        if (!std::filesystem::exists(fileLocation)) {
            throw std::runtime_error("file does not exists: " + fileLocation);
        }

        if (isSpirV(fileName)) {
            const auto source = readFile(fileLocation);
            try {
                return device->createShaderModuleUnique({
                    vk::ShaderModuleCreateFlags(),
                    source.size(),
                    reinterpret_cast<const uint32_t*>(source.data())
                    });
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to create shader module for " + fileLocation);
            }
        }
        else {
            const std::string sourceString = readTextFile(fileLocation); // Read shader text
            const shaderc_shader_kind kind = getFileKind(fileLocation);	// Determine shader type from extension

            // Setup compiler environment
            shaderc::Compiler compiler;
            shaderc::CompileOptions compileOptions;
            compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
            compileOptions.SetSourceLanguage(shaderc_source_language_glsl);
            if (optimize) {
                compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
            }

            // modified loading of glsl -> spir-v
            // https://github.com/google/shaderc
            // NOTE: need to download debug libs for vulkan sdk (https://files.lunarg.com/SDK-1.2.170.0/)
            auto result = compiler.PreprocessGlsl(sourceString, kind, fileLocation.data(), compileOptions); // Preprocess shader file
            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                throw std::runtime_error("failed to preprocess glsl shader " + result.GetErrorMessage());
            }

            auto module = compiler.CompileGlslToSpv({ result.begin(), result.end() }, kind, fileLocation.data(), compileOptions);  // Compile GLSL to SPIR-V
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                throw std::runtime_error("failed to compile glsl shader " + result.GetErrorMessage());
            }

            std::vector<uint32_t> binary{ module.cbegin(), module.cend() };			 

            try {
                return device->createShaderModuleUnique({
                    vk::ShaderModuleCreateFlags(),
                    binary.size() * sizeof(uint32_t),
                    binary.data()
                    });
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to create shader module for " + fileLocation);
            }
        }
    }

    bool isSpirV(const std::string_view& fileName) {
        const std::string_view extention = fileName.substr(fileName.size() - 4, 4);
        return extention == ".spv";
    }

    shaderc_shader_kind getFileKind(std::string fileName) {
        // Get file extension
        const auto dotIndex = fileName.find_last_of('.');
        const auto extension = fileName.substr(dotIndex + 1);

        // Fall-back in case we cannot infer type from extension
        shaderc_shader_kind kind = shaderc_shader_kind::shaderc_glsl_infer_from_source;

        if (extension == "comp")
            kind = shaderc_shader_kind::shaderc_glsl_default_compute_shader;
        else if (extension == "vert")
            kind = shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
        else if (extension == "frag")
            kind = shaderc_shader_kind::shaderc_glsl_default_fragment_shader;
        else if (extension == "geom")
            kind = shaderc_shader_kind::shaderc_glsl_default_geometry_shader;

        return kind;
    }

    void createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<vk::ImageView, 3> attachments{
                colorImageView,
                depthImageView,
                swapChainImageViews[i],
            };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.renderPass = renderPass; // renderpass that is compatiable with this framebuffer (same # of attachments and same attachment types)
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data(); // bound the image views to the attachments in the renderpass
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1; // number of layers in the image view

            try {
                swapChainFramebuffers[i] = device->createFramebuffer(framebufferInfo);
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void createCommandPool() {
        // Manage memory of command buffers

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        try {
            commandPool = device->createCommandPool(poolInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void createColorResources() {
        vk::Format colorFormat = swapChainImageFormat;

        // 1 mip mapped have to be used for multi sampling
        createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, colorImage, colorImageMemory);
        colorImageView = createImageView(colorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
    }

    void createDepthResources() {
        vk::Format depthFormat = findDepthFormat();

        createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1);

        //transitionImageLayout(depthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    bool hasStencilComponent(vk::Format format) {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    vk::Format findDepthFormat() {
        return findSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {

        for (vk::Format format : candidates) {
            vk::FormatProperties props = physicalDevice.getFormatProperties(format);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        // Specify region to copy  to image
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        // We want to copy color
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        // We want the whole image
        region.imageOffset = vk::Offset3D( 0, 0, 0 );
        region.imageExtent = vk::Extent3D(width, height, 1);

        // Copy buffer to image that has the transfer destination optimal layout
        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

        endSingleTimeCommands(commandBuffer);
    }

    void createTextureImage() {
        int texWidth, texHeight, texChannels;
        // Load image with alpha as array of pixel values row x row with 4 bytes per pixel
        stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        vk::DeviceSize imageSize = texWidth * texHeight * 4;

        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1; // times texture size can be divisible by 2

        if (!pixels) {
            std::cout << stbi_failure_reason() << std::endl;
            throw std::runtime_error("failed to load texture image!");
        }

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        // Create buffer on host so we can map the texture to it and transfer it later on
        createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        // Copy image data to staging buffer
        void* data = device->mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        device->unmapMemory(stagingBufferMemory);

        // Clean up image arrary
        stbi_image_free(pixels);
        
        createImage(texWidth, texHeight, mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, 
            vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal,
            textureImage, textureImageMemory);

        // Transition image to the optimal layout and copy buffer to image. old layout must match the layout we used to create the image
        transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, mipLevels);
        copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        // Transfer it to shader read only so the shader can access it
        //transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1);
        generateMipmaps(textureImage, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight, mipLevels);

        device->destroyBuffer(stagingBuffer);
        device->freeMemory(stagingBufferMemory);
    }

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) {
        vk::ImageCreateInfo imageInfo{};
        imageInfo.imageType = vk::ImageType::e2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels; // mip mapping level
        imageInfo.arrayLayers = 1;
        imageInfo.format = format; // image color space
        imageInfo.tiling = tiling; // tiling
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage = usage; // Image data is transfered to this image and we are using this image to sample from in the shader
        imageInfo.sharingMode = vk::SharingMode::eExclusive; // Image only used by one queue family (graphics)
        imageInfo.samples = numSamples; // Multisampling 
        //imageInfo.flags = vk::ImageCreateFlagBits::eSparseBinding; // Optional; sparse images where certain regions in the texture are backed by memory. ie. 3d voxel textures

        try {
            image = device->createImage(imageInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create image!");
        }

        // allocate memory for the image in gpu
        vk::MemoryRequirements memRequirements = device->getImageMemoryRequirements(image);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memRequirements.size;
        // driver may not copy buffer data immediately into memory so we use a memory heap that is host coherent
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        // TODO: instead of allocating memory for every buffer we should use a custom allocator that splits ups a single allocation among lots of objects using offsets.
        // Max # of allocations (maxMemoryAllocationCount) is different from each device and can be as low as 4096 times on high end devices that can simultaneously exist
        // https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        try {
            imageMemory = device->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        device->bindImageMemory(image, imageMemory, 0);
    }

    void createTextureImageView() {
        textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, mipLevels);
    }

    void createTextureSampler() {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear; // oversampling solution
        samplerInfo.minFilter = vk::Filter::eLinear; // undersampling solution (anisotropic filtering)
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat; // repeat texture going beyond image dimensions
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack; // color when sampled outside texture for clamp to border mode
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = physicalDevice.getProperties().limits.maxSamplerAnisotropy; // Use the maximum ansiotropy level
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // if true use coords [0,0] -> [width, height] instead of 0-1 range
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways; // If a comparison function is enabled, then texels will first be compared to a value, and the result of that comparison is used in filtering operations.
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear; // mip mapping
        samplerInfo.mipLodBias = 1.0f; // weight for lod (scale)
        samplerInfo.minLod = 0.0f; // Min lod for the texture
        samplerInfo.maxLod = static_cast<float>(mipLevels);

        try {
            textureSampler = device->createSampler(samplerInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
        // Check if image format supports linear blitting
        vk::FormatProperties formatProperties = physicalDevice.getFormatProperties(imageFormat);
        // using optimalTilingFeatures since we created an image with optimal tiling
        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1; // wait for previous to be done
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, // Which operations occur before the barrier and which should wait on the barrier
                {},  // Allow per region to already begin reading from the parts of a resource that were written so far 
                0, nullptr, // memory barriers
                0, nullptr, // buffer barriers
                1, &barrier // image barreirs
            );

            vk::ImageBlit blit{};
            blit.srcOffsets[0] = vk::Offset3D( 0, 0, 0 ); // take the whole image
            blit.srcOffsets[1] = vk::Offset3D( mipWidth, mipHeight, 1 ); // take the whole image
            blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            blit.srcSubresource.mipLevel = i - 1; // Use the previous image as source
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = vk::Offset3D(0, 0, 0 );
            blit.dstOffsets[1] = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 ); // put the source region into this region (half of source)
            blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            commandBuffer.blitImage(
                image, vk::ImageLayout::eTransferSrcOptimal,
                image, vk::ImageLayout::eTransferDstOptimal,
                1, &blit,
                vk::Filter::eLinear); // Do linear interp for blending of mip maps

            // Transfer the image to fragement shader dest
            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal; // src since barrier is still in src optimal layout in the transfer stage)
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, // Which operations occur before the barrier and which should wait on the barrier
                {},  // Allow per region to already begin reading from the parts of a resource that were written so far 
                0, nullptr, // memory barriers
                0, nullptr, // buffer barriers
                1, &barrier // image barreirs
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        // Transfer the last image to fragement shader dest
        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, // Which operations occur before the barrier and which should wait on the barrier
            {},  // Allow per region to already begin reading from the parts of a resource that were written so far 
            0, nullptr, // memory barriers
            0, nullptr, // buffer barriers
            1, &barrier // image barreirs
        );

        endSingleTimeCommands(commandBuffer);
    }

    vk::SampleCountFlagBits getMaxUsableSampleCount() {
        vk::SampleCountFlags counts = physicalDevice.getProperties().limits.framebufferColorSampleCounts & physicalDevice.getProperties().limits.framebufferDepthSampleCounts;
        if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
        if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
        if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
        if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
        if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
        if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

        return vk::SampleCountFlagBits::e1;
    }

    void loadModel() {
        tinyobj::attrib_t attrib; // positions, normals, tex coords
        std::vector<tinyobj::shape_t> shapes; // contains all the objects with their faces 
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
    
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // vulkan 0 is at top left instead of bottom left
                };
                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) { // Store vertex index if pos + texture coords are not already in vertices
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void rebuffer() {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        // Copy data into staging buffer 
        void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize); // offset, size
        memcpy(data, vertices.data(), (size_t)bufferSize);
        device->unmapMemory(stagingBufferMemory);

        // Copy buffer into actual vertex buffer
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        device->destroyBuffer(stagingBuffer);
        device->freeMemory(stagingBufferMemory);
    }

    void createVertexBuffer() {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        // Copy data into staging buffer 
        void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize); // offset, size
        memcpy(data, vertices.data(), (size_t)bufferSize);
        device->unmapMemory(stagingBufferMemory);

        // Create buffer to transfer to and use as vertex buffer
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

        // Copy buffer into actual vertex buffer
        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        device->destroyBuffer(stagingBuffer);
        device->freeMemory(stagingBufferMemory);
    }

    void createIndexBuffer() {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        // Copy data into staging buffer 
        void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize); // offset, size
        memcpy(data, indices.data(), (size_t)bufferSize);
        device->unmapMemory(stagingBufferMemory);

        // Create buffer to transfer to and use as index buffer
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

        // Copy buffer into actual index buffer
        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        device->destroyBuffer(stagingBuffer);
        device->freeMemory(stagingBufferMemory);
    }

    void createUniformBuffers() {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapChainImages.size());
        uniformBuffersMemory.resize(swapChainImages.size());

        // No need to map memory here since we are going to pass the uniform each frame so we need to map it each frame in draw

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = useCamera ? camera.getViewMatrix() : glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = useCamera ? glm::perspective(glm::radians(camera.zoom), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f) :  glm::perspective(glm::radians(80.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 3000.0f);
        ubo.proj[1][1] *= -1; // flip image 

        // Copy data into mapped memory
        void* data = device->mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo)); // offset, size
        memcpy(data, &ubo, sizeof(ubo));
        device->unmapMemory(uniformBuffersMemory[currentImage]);
    }

    void createDescriptorPool() {
        // Create a pool to allocate the descriptor sets to
        std::array<vk::DescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size()); // max sets allocated
        //poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

        try {
            descriptorPool = device->createDescriptorPool(poolInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSetLayout() {
        vk::DescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex; // notify we are using it in vertex stage
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment; // notify we are using it in fragment stage
        samplerLayoutBinding.pImmutableSamplers = nullptr; // Optional

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings{ uboLayoutBinding, samplerLayoutBinding };
        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        try {
            descriptorSetLayout = device->createDescriptorSetLayout(layoutInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createDescriptorSets() {
        // allocate descriptor sets for each swapchain image with the same layout
        std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        // allocate the sets based on the layout
        try {
            descriptorSets.resize(swapChainImages.size());
            device->allocateDescriptorSets(&allocInfo, descriptorSets.data());
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            // create buffer info for uniform buffer
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            // image info for sampler
            vk::DescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal; // layout of the image
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = textureSampler;

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].dstSet = descriptorSets[i]; // update this descriptor set
            descriptorWrites[0].dstBinding = 0; // update the set at binding 0
            descriptorWrites[0].dstArrayElement = 0; // update the first set in the array
            descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer; // type
            descriptorWrites[0].descriptorCount = 1; // update only 1 set
            descriptorWrites[0].pBufferInfo = &bufferInfo; // for buffer
            descriptorWrites[0].pImageInfo = nullptr; // Optional (for image data)
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional (for buffer views)

            descriptorWrites[1].dstSet = descriptorSets[i]; // update this descriptor set
            descriptorWrites[1].dstBinding = 1; // update the set at binding 1
            descriptorWrites[1].dstArrayElement = 0; // update the first set in the array
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler; // type
            descriptorWrites[1].descriptorCount = 1; // update only 1 set
            descriptorWrites[1].pImageInfo = &imageInfo; // for image

            device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr); // update using writes and copy
        }
    }

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
        vk::BufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive; // Only allow graphics queue family to use this buffer

        try {
            buffer = device->createBuffer(bufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create buffer!");
        }
        // allocate memory for the buffer in gpu
        vk::MemoryRequirements memRequirements = device->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memRequirements.size;
        // driver may not copy buffer data immediately into memory so we use a memory heap that is host coherent
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        // TODO: instead of allocating memory for every buffer we should use a custom allocator that splits ups a single allocation among lots of objects using offsets.
        // Max # of allocations (maxMemoryAllocationCount) is different from each device and can be as low as 4096 times on high end devices that can simultaneously exist
        // https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        try {
            bufferMemory = device->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        device->bindBufferMemory(buffer, bufferMemory, 0);
    }
    
    vk::CommandBuffer beginSingleTimeCommands() {
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer = device->allocateCommandBuffers(allocInfo)[0];

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }
    
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Execute command
        graphicsQueue.submit(submitInfo, nullptr);
        graphicsQueue.waitIdle(); // Wait until queue becomes idle. Could use a fence to do simultaneous command buffer transfers and wait for them to all complete

        device->freeCommandBuffers(commandPool, commandBuffer);
    }
    
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        // Use a command buffer to transfer between buffers. Should probably use a different command pool since the buffer is short-lived
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::BufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mipLevels) {
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::ImageMemoryBarrier barrier{};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Used to transfer image ownership over queues
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        // The specific part of the image
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
            }
        }
        else {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor; // Affects color
        }

        // Handle the two transitions fore the image
        // undefined -> transfer dest optimal
        // transfer dest optimal -> shader read only optimal (see createTextureImage)
        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            barrier.srcAccessMask = {};
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(
            sourceStage, destinationStage, // Which operations occur before the barrier and which should wait on the barrier
            {},  // Allow per region to already begin reading from the parts of a resource that were written so far 
            0, nullptr, // memory barriers
            0, nullptr, // buffer barriers
            1, &barrier // image barreirs
        );


        endSingleTimeCommands(commandBuffer);
    }

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
        vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
        // Find suitable memory based on type and properties
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void createCommandBuffers() {
        commandBuffers.resize(swapChainFramebuffers.size());

        vk::CommandBufferAllocateInfo allocInfo{}; // Allocate commad buffers
        allocInfo.commandPool = commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary; // Secondary cannot be submitted to queue for execution but can be called by primary
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        try {
            commandBuffers = device->allocateCommandBuffers(allocInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            vk::CommandBufferBeginInfo beginInfo{};
            beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse; // ONE_TIME_SUBMIT -> command rerecorded; RENDER_PASS_CONTINUE -> entirely in renderpass; SIMULTANEOUS -> resubmitted while it is also pending
            //beginInfo.pInheritanceInfo = nullptr; // Optional // for secondary buffers to inherit state from

            try {
                commandBuffers[i].begin(beginInfo);
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            vk::RenderPassBeginInfo renderPassInfo{}; // begin render pass
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = vk::Offset2D(0, 0); // render area
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<vk::ClearValue, 2> clearValues{}; // Order of clear values should be in the order of the attachments
            clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 0.1f, 1.0f }); // color for vk::AttachmentLoadOp::eClear in the attachment
            clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f); // depth and stencil clear values
            
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline); // Render pass commands will be executed in primary buffer (inline)

            commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline); // Bind graphics pipeline and type (graphics, compute, raytracing)

            // Setup viewport
            vk::Viewport viewport{}; // Describes the section of the window to draw
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)swapChainExtent.width;
            viewport.height = (float)swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vk::Rect2D scissor{}; // Describes the section of the viewport to include. Anything outside is discarded by the rasterizer
            scissor.offset = vk::Offset2D(0, 0);
            scissor.extent = swapChainExtent;

            commandBuffers[i].setViewport(0, 1, &viewport);
            commandBuffers[i].setScissor(0, 1, &scissor);

            commandBuffers[i].setLineWidth(1.0f);

            // Draw 
            vk::Buffer vertexBuffers[]{ vertexBuffer };
            vk::DeviceSize offsets[]{ 0 };
            commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets); // # of offsets and bindings, buffers, and starting offsets
            commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
            // bind to graphics pipeline with the layout set is based on. Specify start, size, and sets. Last two specify array of offsets for the sets
            commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

            commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0); // indices count, instance count (instanced rendering), first index, first vertex offset, first instance offset
            //commandBuffers[i].draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);  // vertex count, instance count (instanced rendering), first vertex offset, first instance offset

            commandBuffers[i].endRenderPass();

            try {
                commandBuffers[i].end();
            }
            catch (vk::SystemError err) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), nullptr);

        try {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                imageAvailableSemaphores[i] = device->createSemaphore({});
                renderFinishedSemaphores[i] = device->createSemaphore({});
                inFlightFences[i] = device->createFence({ vk::FenceCreateFlagBits::eSignaled }); // Set fence to signaled state so the drawFrame will not wait on init
            }
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    void drawFrame() {
        // Aquire image from swapchain, attach image as color attachment to framebuffer and execute command buffer, return image to swapchain for presentation
        // These calls are async so they will return even though they are not done so we must use fences and semaphores
        // Fences are used to sync rendering operations with cpu operations while semaphores can be used to sync commands across command queues

        device->waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max()); // Wait until 1 frame finished, all, timeout

        uint32_t imageIndex;
        try {
            vk::ResultValue result = device->acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(),
                imageAvailableSemaphores[currentFrame], nullptr); // specify swap chain, timeout, semaphore to signal and or fence. Outputs index of swapChainImages (vk::Iamge)
            imageIndex = result.value;
        }
        catch (vk::OutOfDateKHRError err) {
            recreateSwapChain(); // must rebuild swapchain
            return;
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(imageIndex);

        // Check if a previous frame is using this image (i.e. there is a fence to wait on). Shouldn't do this if the command buffers are rerecorded each frame.(https://www.reddit.com/r/vulkan/comments/jtuhmu/synchronizing_frames_in_flight/gcfedfy/)
        if (imagesInFlight[imageIndex] != vk::Fence()) {
            device->waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
        }
        // Mark the image as now being in use by this frame
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        vk::SubmitInfo submitInfo{}; // Setup queue submission

        vk::Semaphore waitSemaphores[]{ imageAvailableSemaphores[currentFrame] }; // Wait for these semaphores before execution. Each semaphore matches with the corresponding index of wait stages
        vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Wait on these stages. Wait until the image is ready
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1; // The command buffers to submit
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        vk::Semaphore signalSemaphores[]{ renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores; // Signal these semaphores when execution finishes

        device->resetFences(1, &inFlightFences[currentFrame]); // manually restore fence to default state

        try {
            graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]); // Send command buffer to graphics queue and signal fence when command buffers finished
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        vk::PresentInfoKHR presentInfo{}; // Present info to submit back to swapchain to show on screen
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores; // Wait until render semaphores are signaled

        vk::SwapchainKHR swapChains[]{ swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains; // Swap chain to present image to
        presentInfo.pImageIndices = &imageIndex; // index(s) of swapchain images to use
        presentInfo.pResults = nullptr; // Optional. Get array of vk::Result to check for each swap chain if presentation was successful

        vk::Result resultPresent;
        try {
            resultPresent = presentQueue.presentKHR(presentInfo); // Submit info to presentQueue screen to show it
        }
        catch (vk::OutOfDateKHRError err) {
            resultPresent = vk::Result::eErrorOutOfDateKHR;
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        if (resultPresent == vk::Result::eErrorOutOfDateKHR || resultPresent == vk::Result::eSuboptimalKHR || framebufferResized) {
            std::cout << "swap chain out of date/suboptimal/window resized - recreating" << std::endl;
            framebufferResized = false;
            recreateSwapChain(); // must recreate swapchain even if the old is suboptimal since we want the best image
            return;
        }


        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    

    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        vk::DispatchLoaderDynamic dldi(*instance, vkGetInstanceProcAddr);

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
        //debugMessenger = instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);

        // NOTE: reinterpret_cast is also used by vulkan.hpp internally for all these structs
        if (CreateDebugUtilsMessengerEXT(*instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug callback!");
        }
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        std::vector<vk::ExtensionProperties> available_extensions = vk::enumerateInstanceExtensionProperties();

        std::cout << "available extensions:\n";

        for (const auto& extension : available_extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        std::cout << "glfw required extensions:\n";
        for (int i = 0; i < glfwExtensionCount; ++i) {

            std::cout << '\t' << glfwExtensions[i] << '\n';
        }

        if (!validateExtensions(glfwExtensions, &glfwExtensionCount, &extensions)) {
            throw std::runtime_error("Extensions not available!");
        }

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        std::cout << "required extensions:\n";
        for (const auto& extension : extensions) {
            std::cout << '\t' << extension << '\n';
        }

        return extensions;
    }

    bool validateExtensions(const char** req_extensions, uint32_t* req_count, std::vector<const char*>* available_extensions) {
        for (auto i = 0; i < *req_count; ++i) {
            std::string_view name = req_extensions[i];
            if (std::find_if(available_extensions->begin(), available_extensions->end(), [&name](const char*& obj) {return obj == name; }) == available_extensions->end()) return false;
        }
        return true;
    }
    bool checkValidationLayerSupport() {
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        std::set<std::string_view> requiredLayers(validationLayers.begin(), validationLayers.end());

        for (const auto& layer : availableLayers) {
            requiredLayers.erase(layer.layerName);
        }

        return requiredLayers.empty();
    }

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        vk::DebugUtilsMessageTypeFlagBitsEXT messageType,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::string_view severity;
        switch (messageSeverity) {
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
        std::string_view mtype;
        switch (messageType) {
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
            mtype = "[GENERAL] ";
            break;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
            mtype = "[VALIDATION] ";
            break;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
            mtype = "[PERFOMANCE] ";
            break;
        default:
            mtype = "[?] ";
        }
        std::cerr << severity << mtype << pCallbackData->pMessage << std::endl;

        return false;
    }

    static std::vector<char> readFile(const std::string_view& filename) {
        std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
    static std::string readTextFile(const std::string_view& fileName)
    {
        std::string buffer;
        std::ifstream file(fileName.data());
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        std::string temp;
        while (getline(file, temp))
            buffer.append(temp), buffer.append("\n");

        file >> buffer;
        file.close();
        return buffer;
    }
};

int main() {

    std::cout << vk::enumerateInstanceExtensionProperties().size() << " extensions supported\n";

    /*HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }*/

    VulkanEngine engine;
 
    engine.init();

    engine.run();

    engine.cleanup();

    return EXIT_SUCCESS;
}