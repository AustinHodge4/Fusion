#include <engine/vk_engine.hpp>
#include <GLFW/glfw3.h>

//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#include <engine/vk_initializers.hpp>
#include <engine/vk_debugger.hpp>
#include <engine/vk_swapchain.hpp>
#include <engine/vk_shader.hpp>
#include <engine/vk_pipeline.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static std::vector<char> read_file(std::string_view p_filename);
static std::string read_text_file(std::string_view p_fileName);

void VulkanEngine::init()
{
    
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(
        m_windowExtent.width, 
        m_windowExtent.height, 
        "Vulkan Engine", 
        nullptr, 
        nullptr
    );
    glfwSetWindowUserPointer(m_window, this); // store app pointer so we can use it in the callback
    glfwSetKeyCallback(m_window, key_callback);

    init_vulkan();

    init_swapchain();

    init_commands();

    init_default_renderpass();

    init_framebuffers();

    init_sync_structures();

    init_pipelines();

    m_isInitialized = true;
}

void VulkanEngine::cleanup()
{
    if (m_isInitialized) {
        m_device->destroyPipeline(m_graphicsPipeline);
        m_device->destroyPipeline(m_colorGraphicsPipeline);
        m_device->destroyPipelineLayout(m_pipelineLayout);

        m_device->destroyRenderPass(m_renderPass);

        m_device->freeCommandBuffers(m_commandPool, m_mainCommandBuffer);
        m_device->destroyCommandPool(m_commandPool);

        m_device->destroySemaphore(m_presentSemaphore);
        m_device->destroySemaphore(m_renderSemaphore);
        m_device->destroyFence(m_renderFence);

        for (auto framebuffer : m_swapchainFramebuffers) {
            m_device->destroyFramebuffer(framebuffer);
        }

        for (auto imageView : m_swapchainImageViews) {
            m_device->destroyImageView(imageView);
        }
        // not using UniqeSwapchain to destroy in correct order - before the surface
        m_device->destroySwapchainKHR(m_swapchain);

        m_instance->destroySurfaceKHR(m_surface);
        if (vk_debug::m_enableValidationLayers) {
            vk_debug::destroy_debug_utils_messenger_ext(*m_instance, m_debugMessenger, nullptr);
        }

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void VulkanEngine::draw()
{
    m_device->waitForFences(1, &m_renderFence, VK_TRUE, std::numeric_limits<uint64_t>::max()); // Wait until 1 frame finished, all, timeout (ns)

    uint32_t imageIndex;
    try {
        vk::ResultValue result = m_device->acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(),
            m_presentSemaphore, nullptr); // specify swap chain, timeout, semaphore to signal and or fence. Outputs index of m_swapchainImages (vk::Iamge)
        imageIndex = result.value;
    }
    //catch (vk::OutOfDateKHRError err) {
    //    recreateSwapChain(); // must rebuild swapchain
    //    return;
    //}
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Rebuild command buffer
    try {
        m_mainCommandBuffer.reset();
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to reset command buffer");
    }

    vk::CommandBufferBeginInfo beginInfo = vk_init::command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    try {
        m_mainCommandBuffer.begin(beginInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    vk::RenderPassBeginInfo renderPassInfo = vk_init::renderpass_begin_info(m_renderPass, m_swapchainExtent, m_swapchainFramebuffers[imageIndex]); // begin render pass

    float flash = abs(sin(m_frameNumber / 120.f));
    std::array<vk::ClearValue, 2> clearValues{}; // Order of clear values should be in the order of the attachments
    clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{ 0.1f, 0.1f, 0.1f, 1.0f }); // color for vk::AttachmentLoadOp::eClear in the attachment
    clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f); // depth and stencil clear values

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    m_mainCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline); // Render pass commands will be executed in primary buffer (inline)

    if (m_selectedShader == 0) {
        m_mainCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline); // Bind graphics pipeline and type (graphics, compute, raytracing)
    }
    else {
        m_mainCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_colorGraphicsPipeline); // Bind graphics pipeline and type (graphics, compute, raytracing)
    }

    // Setup viewport
    vk::Viewport viewport{}; // Describes the section of the window to draw
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchainExtent.width;
    viewport.height = (float)m_swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor{}; // Describes the section of the viewport to include. Anything outside is discarded by the rasterizer
    scissor.offset = vk::Offset2D(0, 0);
    scissor.extent = m_swapchainExtent;

    m_mainCommandBuffer.setViewport(0, 1, &viewport);
    m_mainCommandBuffer.setScissor(0, 1, &scissor);

    m_mainCommandBuffer.setLineWidth(1.0f);

    // Draw 
    m_mainCommandBuffer.draw(3, 1, 0, 0);

    m_mainCommandBuffer.endRenderPass();

    try {
        m_mainCommandBuffer.end();
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to record command buffer!");
    }


    vk::SubmitInfo submitInfo = vk_init::submit_info(&m_mainCommandBuffer); // Setup queue submission

    vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput }; // Wait on these stages. Wait until the image is ready
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_presentSemaphore; // wait until image ready on gpu
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_renderSemaphore; // Signal these semaphores when submit execution finishes

    m_device->resetFences(1, &m_renderFence); // manually restore fence to default state

    try {
        m_graphicsQueue.submit(submitInfo, m_renderFence); // Send command buffer to graphics queue and signal fence when command buffers finished
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo{}; // Present info to submit back to swapchain to show on screen
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_renderSemaphore; // Wait until render semaphores are signaled

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain; // Swap chain to present image to
    presentInfo.pImageIndices = &imageIndex; // index(s) of swapchain images to use
    presentInfo.pResults = nullptr; // Optional. Get array of vk::Result to check for each swap chain if presentation was successful

    vk::Result resultPresent;
    try {
        resultPresent = m_graphicsQueue.presentKHR(presentInfo); // Submit info to presentQueue screen to show it
    }
    catch (vk::OutOfDateKHRError err) {
        resultPresent = vk::Result::eErrorOutOfDateKHR;
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_frameNumber++;
    //currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanEngine::run()
{
    while (!glfwWindowShouldClose(m_window)) {
        auto current = glfwGetTime();
        m_deltaTime = current - m_lastTime;
        m_lastTime = current;
        //std::cout << "dt: " << _deltaTime << std::endl;

        glfwPollEvents();
        // handleInput();
        draw();
    }
    m_device->waitIdle();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
        app->m_selectedShader = (app->m_selectedShader + 1) % 2;
    }
}

void VulkanEngine::init_vulkan()
{
    if (vk_debug::m_enableValidationLayers && !vk_debug::check_validation_layer_support()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "Vulkan Engine Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Austin's Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = get_required_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (vk_debug::m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vk_debug::m_validationLayers.size());
        createInfo.ppEnabledLayerNames = vk_debug::m_validationLayers.data();

        vk_debug::populate_debug_messenger_create_info(debugCreateInfo, vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
        //populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    try {
        m_instance = vk::createInstanceUnique(createInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create instance!");
    }

    vk_debug::setup_debug_messenger(m_instance.get(), m_debugMessenger, vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    m_surface = rawSurface;

    vk_device::PhysicalDeviceSelector selector{ m_instance.get(), m_surface };
    selector.add_required_extensions(m_deviceExtensions);
    vk::PhysicalDeviceFeatures requiredDeviceFeatures{};
    requiredDeviceFeatures.wideLines = true;
    requiredDeviceFeatures.geometryShader = true;
    requiredDeviceFeatures.fillModeNonSolid = true;
    requiredDeviceFeatures.samplerAnisotropy = true;
    selector.set_required_features(requiredDeviceFeatures);
    m_physicalDevice = selector.select();

    m_queueFamilies = vk_device::find_queue_families(m_physicalDevice, m_surface);
    
    m_device = vk_device::create_logical_device(m_physicalDevice, m_surface, m_queueFamilies, requiredDeviceFeatures, m_deviceExtensions);

    m_graphicsQueue = m_device->getQueue(m_queueFamilies.graphicsFamily, 0);
    m_presentQueue = m_device->getQueue(m_queueFamilies.presentFamily, 0);
}

void VulkanEngine::init_swapchain()
{
    vk_swapchain::SwapchainInfo info{};
    info.m_physicalDevice = m_physicalDevice;
    info.m_device = m_device.get();
    info.m_surface = m_surface;
    info.m_desiredSurfaceFormats = std::vector<vk::SurfaceFormatKHR>{ { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear } };
    info.m_desiredPresentModes = std::vector<vk::PresentModeKHR>{ vk::PresentModeKHR::eMailbox };
    info.m_fallbackPresentMode = vk::PresentModeKHR::eFifo;
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    info.m_desiredWidth = static_cast<uint32_t>(width);
    info.m_desiredHeight = static_cast<uint32_t>(height);
    info.m_imageUsageFlags = vk::ImageUsageFlagBits::eColorAttachment;
    info.m_graphicsQueueIndex = m_queueFamilies.graphicsFamily;
    info.m_presentQueueIndex = m_queueFamilies.presentFamily;
    info.m_compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    info.m_clipped = true;
    info.m_oldSwapchain = nullptr;

    vk_swapchain::SwapchainDetails details = vk_swapchain::create_swapchain(info);
    m_swapchain = details.m_swapchain;
    m_swapchainExtent = details.m_swapchainExtent;
    m_swapchainImageFormat = details.m_swapchainImageFormat;
    m_swapchainImages = details.m_swapchainImages;
    m_swapchainImageViews = vk_swapchain::create_image_views(m_device.get(), details);
}

void VulkanEngine::init_commands()
{
    //  allow for resetting of individual command buffers
    vk::CommandPoolCreateInfo poolInfo = vk_init::command_pool_create_info(m_queueFamilies.graphicsFamily, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    try {
        m_commandPool = m_device->createCommandPool(poolInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create command pool!");
    }

    vk::CommandBufferAllocateInfo allocInfo = vk_init::command_buffer_allocate_info(m_commandPool, 1, vk::CommandBufferLevel::ePrimary);
    m_mainCommandBuffer = m_device->allocateCommandBuffers(allocInfo)[0];
}

void VulkanEngine::init_default_renderpass()
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapchainImageFormat; // should match swapchain image format
    colorAttachment.samples = vk::SampleCountFlagBits::e1; // multi-samples
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // What to do with color data in attachment before rendering. Clear it before drawing new frame
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // What to do with color data in attachment after rendering. Store it so we can see it
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined; // don't care. Transform image to another layout. Where initial is incoming input and final is output
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR; // ready to present (vk::ImageLayout::ePresentSrcKHR) multisampled images need to be resolved to a regular image first

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // Refrence index of attachment from pAttachments array (see renderPassInfo). This case is 0 since this is the first in the array
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal; // Layout to transform attachment to during a subpass which has this attachment

    //vk::AttachmentDescription depthAttachment{};
    //depthAttachment.format = findDepthFormat();
    //depthAttachment.samples = msaaSamples; // multi-samples
    //depthAttachment.loadOp = vk::AttachmentLoadOp::eClear; // What to do with depth data in attachment before rendering. Clear it before drawing new frame
    //depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare; // What to do with depth data in attachment after rendering
    //depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    //depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    //depthAttachment.initialLayout = vk::ImageLayout::eUndefined; // Transform image to another layout. Where initial is incoming input and final is output
    //depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    //vk::AttachmentReference depthAttachmentRef{};
    //depthAttachmentRef.attachment = 1; // Refrence index of attachment from pAttachments array (see renderPassInfo).
    //depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal; // Layout to transform attachment to during a subpass which has this attachment

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics; // Type of subpass (graphics, compute, raytracing)
    subpass.colorAttachmentCount = 1; // The attachments that should go with this subpass
    subpass.pColorAttachments = &colorAttachmentRef; // The index of the attachment array is directly related to layout(location = #) in fragment shader
    //subpass.pDepthStencilAttachment = &depthAttachmentRef;
    //subpass.pResolveAttachments = &colorAttachmentResolveRef; // use multi sample resolve method

    vk::SubpassDependency dependency{}; // Have the subpass depend on one. By default there are hidden implicit subpasses before and after our defined subpass
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // refers to the implicit subpass before
    dependency.dstSubpass = 0; // our subpass (index in pSubpasses)
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests; // Wait on the color attachment output from swapchain and before fragments
    //dependency.srcAccessMask = 0;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite; // allow read and write for our subpass

    std::array<vk::AttachmentDescription, 1> attachments = { colorAttachment};
    vk::RenderPassCreateInfo renderPassInfo{}; // create render pass with array of attachments and subpasses
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    try {
        m_renderPass = m_device->createRenderPass(renderPassInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanEngine::init_framebuffers()
{
    m_swapchainFramebuffers.resize(m_swapchainImageViews.size());

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        std::array<vk::ImageView, 1> attachments{
            m_swapchainImageViews[i],
        };

        vk::FramebufferCreateInfo framebufferInfo = vk_init::framebuffer_create_info(m_renderPass, m_swapchainExtent);
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data(); // bound the image views to the attachments in the renderpass

        try {
            m_swapchainFramebuffers[i] = m_device->createFramebuffer(framebufferInfo);
        }
        catch (vk::SystemError err) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanEngine::init_sync_structures()
{
    try {
        m_renderSemaphore = m_device->createSemaphore({});
        m_presentSemaphore = m_device->createSemaphore({});
        m_renderFence = m_device->createFence({ vk::FenceCreateFlagBits::eSignaled }); // Set fence to signaled state so the drawFrame will not wait on init
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void VulkanEngine::init_pipelines()
{
    vk::UniqueShaderModule triangleVertexShader = load_shader_module("test.vert", true);
    vk::UniqueShaderModule triangleFragShader = load_shader_module("test.frag", true);
    vk::UniqueShaderModule colorTriangleVertexShader = load_shader_module("test-2.vert", true);
    vk::UniqueShaderModule colorTriangleFragShader = load_shader_module("test-2.frag", true);
  
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{}; // Pass any uniforms to shaders
    pipelineLayoutInfo.setLayoutCount = 0;
    //pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Tell the pipeline we are using descriptor set layouts for this shader
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    try {
        m_pipelineLayout = m_device->createPipelineLayout(pipelineLayoutInfo);
    }
    catch (vk::SystemError err) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    vk_pipeline::PipelineConfig piplineConfig;
    piplineConfig.m_shaderStages.push_back(
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eVertex, // pipeline stage (vertex)
            *triangleVertexShader,
            "main" //entry point
        }
    );
    piplineConfig.m_shaderStages.push_back(
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eFragment, 
            *triangleFragShader,
            "main" //entry point
        }
    );

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    piplineConfig.m_vertexInputInfo = vertexInputInfo;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{}; // Describes what kind of geometry to draw
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE; // If true, possible to break up lines and triangles in the _STRIP topology modes by using a special index
    piplineConfig.m_inputAssembly = inputAssembly;

    std::vector<vk::DynamicState> dynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth,
        vk::DynamicState::eScissor
    };
    piplineConfig.m_dynamicStates = dynamicStates;

    vk::PipelineRasterizationStateCreateInfo rasterizer{}; // Rasterizer; turns into fragments; does depth test, scissor test and face culling
    rasterizer.depthClampEnable = VK_FALSE; // If true, fragments beyond near and far planes are clamped instead of discarded (req GPU feature)
    rasterizer.rasterizerDiscardEnable = VK_FALSE; // If true, geometry never passes this phase
    rasterizer.polygonMode = vk::PolygonMode::eFill; // How fragments are generated for geometry (LINE and POINT req GPU feature)
    rasterizer.lineWidth = 1.0f; // using dynamics. Thickness of fragments. Anything above 1.0 requires GPU feature (wideLines). Maximum is hardware dependent
    rasterizer.cullMode = vk::CullModeFlagBits::eBack; // Culling
    rasterizer.frontFace = vk::FrontFace::eClockwise; // Vertex order for faces
    rasterizer.depthBiasEnable = VK_FALSE; // Can alter depth values
    piplineConfig.m_rasterizer = rasterizer;

    vk::PipelineMultisampleStateCreateInfo multisampling{}; // Multisampling for anti-aliasing
    multisampling.sampleShadingEnable = VK_FALSE; // Improves quality further
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f; // Optional closer to 1 is smooth
    piplineConfig.m_multisampling = multisampling;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{}; // Color blending (make one for each framebuffer)
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    piplineConfig.m_colorBlendAttachment = colorBlendAttachment;

    piplineConfig.m_pipelineLayout = m_pipelineLayout;
    m_graphicsPipeline = piplineConfig.build_pipeline(m_device.get(), m_renderPass);

    piplineConfig.m_shaderStages.clear();
    piplineConfig.m_shaderStages.push_back(
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eVertex, // pipeline stage (vertex)
            *colorTriangleVertexShader,
            "main" //entry point
        }
    );
    piplineConfig.m_shaderStages.push_back(
        {
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eFragment,
            *colorTriangleFragShader,
            "main" //entry point
        }
    );
    m_colorGraphicsPipeline = piplineConfig.build_pipeline(m_device.get(), m_renderPass);
}

vk::UniqueShaderModule VulkanEngine::load_shader_module(std::string_view p_fileName, bool p_optimize)
{
    const std::string fileLocation = "shaders/" + std::string(p_fileName.data());

    // Check if file exists
    if (!std::filesystem::exists(fileLocation)) {
        throw std::runtime_error("file does not exists: " + fileLocation);
    }

    if (vk_shader::is_spir_v(p_fileName)) {
        const auto source = read_file(fileLocation);
        try {
            return m_device->createShaderModuleUnique({
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
        const std::string sourceString = read_text_file(fileLocation); // Read shader text
        const shaderc_shader_kind kind = vk_shader::get_file_kind(fileLocation);	// Determine shader type from extension

        // Setup compiler environment
        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOptions;
        compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        compileOptions.SetSourceLanguage(shaderc_source_language_glsl);
        if (p_optimize) {
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
            return m_device->createShaderModuleUnique({
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

bool VulkanEngine::validate_extensions(const char** p_reqExtensions, const uint32_t* p_reqCount, std::vector<const char*>* p_availableExtensions)
{
    for (auto i = 0; i < *p_reqCount; ++i) {
        std::string_view name = p_reqExtensions[i];
        if (std::find_if(p_availableExtensions->begin(), p_availableExtensions->end(), [&name](const char*& obj) {return obj == name; }) == p_availableExtensions->end()) return false;
    }
    return true;
}

std::vector<const char*> VulkanEngine::get_required_extensions() 
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();

    std::cout << "available extensions:\n";

    for (const auto& extension : availableExtensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    std::cout << "glfw required extensions:\n";
    for (int i = 0; i < glfwExtensionCount; ++i) {

        std::cout << '\t' << glfwExtensions[i] << '\n';
    }

    if (!validate_extensions(glfwExtensions, &glfwExtensionCount, &extensions)) {
        throw std::runtime_error("Extensions not available!");
    }

    if (vk_debug::m_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "required extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension << '\n';
    }

    return extensions;
}

static std::vector<char> read_file(std::string_view p_filename) 
{
    std::ifstream file(p_filename.data(), std::ios::ate | std::ios::binary);

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

static std::string read_text_file(std::string_view p_fileName)
{
    std::string buffer;
    std::ifstream file(p_fileName.data());
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