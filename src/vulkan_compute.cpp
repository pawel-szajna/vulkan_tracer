#include "vulkan_compute.hpp"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>

VulkanCompute::VulkanCompute(usize inputMemorySize,
                             usize outputMemorySize,
                             std::string_view shader,
                             u32 jobsX,
                             u32 jobsY,
                             u32 jobsZ,
                             i32 deviceId)
    : inputMemorySize{inputMemorySize}
    , outputMemorySize{outputMemorySize}
    , jobsX{jobsX}
    , jobsY{jobsY}
    , jobsZ{jobsZ}
{
    SPDLOG_INFO("Creating Vulkan compute helper");

    #if defined(DebugRenderdoc)
    SPDLOG_DEBUG("Setting up RenderDoc API");
    auto *mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
    if (mod == nullptr)
    {
        SPDLOG_INFO("RenderDoc API unavailable");
    }
    else
    {
        auto renderdocGetApi = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
        auto result = renderdocGetApi(eRENDERDOC_API_Version_1_6_0, (void**)&renderdocApi);
        SPDLOG_INFO("RENDERDOC_GetAPI result: {}", result);
    }
    #endif

    SPDLOG_DEBUG("Requested memory: input buffer {} bytes, output buffer {} bytes", inputMemorySize, outputMemorySize);
    SPDLOG_DEBUG("Shader file: {}", shader);

    auto timer = Timers::create("Vulkan setup");

    std::vector<const char*> validationLayers{};
    #if defined(DebugBuild)
    auto availableLayers = vk::enumerateInstanceLayerProperties();
    SPDLOG_DEBUG("Available instance layers:");
    constexpr static std::string_view khronosValidationLayerName{"VK_LAYER_KHRONOS_validation"};
    for (const vk::LayerProperties& layer : availableLayers)
    {
        SPDLOG_DEBUG("    {}: {}", std::string_view(layer.layerName), std::string_view(layer.description));
        if (layer.layerName == khronosValidationLayerName)
        {
            validationLayers.push_back(khronosValidationLayerName.data());
        }
    }
    #endif

    vk::InstanceCreateInfo instanceInfo{vk::InstanceCreateFlags{}, {}, validationLayers, {}};
    instance = vk::createInstance(instanceInfo);

    createDevice(deviceId);
    allocateMemory();
    loadShader(shader);
    createPipeline();

    queue = device.getQueue(queueIndex, 0);
    fence = device.createFence(vk::FenceCreateInfo{});

    SPDLOG_DEBUG("Vulkan setup finished");
}

VulkanCompute::~VulkanCompute()
{
    device.resetCommandPool(commandPool, vk::CommandPoolResetFlags{});
    device.destroyFence(fence);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyPipelineCache(pipelineCache);
    device.destroyShaderModule(shaderModule);
    device.destroyPipeline(computePipeline);
    device.destroyDescriptorPool(descriptorPool);
    device.destroyCommandPool(commandPool);
    device.freeMemory(inputMemory);
    device.freeMemory(outputMemory);
    device.destroyBuffer(inputBuffer);
    device.destroyBuffer(outputBuffer);
    device.destroy();
    instance.destroy();
}

void VulkanCompute::createDevice(i32 deviceId)
{
    auto devices = instance.enumeratePhysicalDevices();

    if (deviceId < 0)
    {
        auto deviceIt = std::find_if(devices.begin(),
                                     devices.end(),
                                     [](const vk::PhysicalDevice& d)
                                     {
                                         auto type = d.getProperties().deviceType;
                                         return type == vk::PhysicalDeviceType::eDiscreteGpu or
                                                type == vk::PhysicalDeviceType::eIntegratedGpu;
                                     });

        if (deviceIt == devices.end())
        {
            throw std::runtime_error("No GPU");
        }

        physicalDevice = *deviceIt;
    }
    else
    {
        if (deviceId >= devices.size())
        {
            throw std::invalid_argument("Invalid device ID provided");
        }

        physicalDevice = *(devices.begin() + deviceId);
    }

    SPDLOG_INFO("Using physical device {}", std::string_view(physicalDevice.getProperties().deviceName));

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    #if defined(DebugBuild)
    SPDLOG_DEBUG("Available queue families:");
    for (const vk::QueueFamilyProperties& family : queueFamilies)
    {
        SPDLOG_DEBUG("    Flags: {}, Queue count: {}", vk::to_string(family.queueFlags), family.queueCount);
    }
    #endif

    auto queueIt =
        std::find_if(queueFamilies.begin(),
                     queueFamilies.end(),
                     [](const vk::QueueFamilyProperties& q) { return q.queueFlags & vk::QueueFlagBits::eCompute; });

    if (queueIt == queueFamilies.end())
    {
        throw std::runtime_error("No compute units");
    }

    queueIndex = std::distance(queueFamilies.begin(), queueIt);

    auto queuePriorities  = {1.0f};
    auto queueCreateInfo  = vk::DeviceQueueCreateInfo{vk::DeviceQueueCreateFlags{}, queueIndex, queuePriorities};
    auto deviceCreateInfo = vk::DeviceCreateInfo{vk::DeviceCreateFlags{}, queueCreateInfo};

    device = physicalDevice.createDevice(deviceCreateInfo);
}

void VulkanCompute::listDevices()
{
    vk::InstanceCreateInfo instanceInfo{vk::InstanceCreateFlags{}, {}, {}, {}};
    auto instance = vk::createInstance(instanceInfo);

    u32 id{};

    SPDLOG_INFO("Available devices:");
    auto devices = instance.enumeratePhysicalDevices();
    for (const auto& dev : devices)
    {
        auto properties = dev.getProperties();
        SPDLOG_INFO("    {}: {} - {}, API version {}.{}",
                    id++,
                    vk::to_string(properties.deviceType),
                    std::string_view(properties.deviceName),
                    VK_VERSION_MAJOR(properties.apiVersion),
                    VK_VERSION_MINOR(properties.apiVersion));
    }

    instance.destroy();
}

void VulkanCompute::allocateMemory()
{
    SPDLOG_DEBUG("Allocating GPU memory");

    vk::BufferCreateInfo inputBufferCreateInfo{vk::BufferCreateFlags{},
                                               inputMemorySize,
                                               vk::BufferUsageFlagBits::eStorageBuffer,
                                               vk::SharingMode::eExclusive,
                                               {}};

    vk::BufferCreateInfo outputBufferCreateInfo{vk::BufferCreateFlags{},
                                                outputMemorySize,
                                                vk::BufferUsageFlagBits::eStorageBuffer,
                                                vk::SharingMode::eExclusive,
                                                {}};

    inputBuffer  = device.createBuffer(inputBufferCreateInfo);
    outputBuffer = device.createBuffer(outputBufferCreateInfo);

    auto inputMemoryReq  = device.getBufferMemoryRequirements(inputBuffer);
    auto outputMemoryReq = device.getBufferMemoryRequirements(outputBuffer);

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    auto memoryTypeIt = std::find_if(memoryProperties.memoryTypes.begin(),
                                     memoryProperties.memoryTypes.end(),
                                     [](const vk::MemoryType& mt)
                                     {
                                         return mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible and
                                                mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent;
                                     });

    if (memoryTypeIt == memoryProperties.memoryTypes.end())
    {
        throw std::runtime_error("No valid memory type");
    }

    std::uint32_t memoryTypeIndex = std::distance(memoryProperties.memoryTypes.begin(), memoryTypeIt);
    auto memoryHeapSize           = memoryProperties.memoryHeaps[memoryTypeIt->heapIndex].size;

    SPDLOG_DEBUG("Video memory heap size: {} MB", memoryHeapSize / 1'048'576);

    vk::MemoryAllocateInfo inAllocateInfo{inputMemoryReq.size, memoryTypeIndex};
    vk::MemoryAllocateInfo outAllocateInfo{outputMemoryReq.size, memoryTypeIndex};
    inputMemory  = device.allocateMemory(inAllocateInfo);
    outputMemory = device.allocateMemory(outAllocateInfo);

    device.bindBufferMemory(inputBuffer, inputMemory, 0);
    device.bindBufferMemory(outputBuffer, outputMemory, 0);
}

void VulkanCompute::loadShader(std::string_view filename)
{
    std::vector<char> shaderContents{};
    std::ifstream shader{std::string(filename), std::ios::binary | std::ios::ate};

    if (shader.bad())
    {
        throw std::runtime_error("No shader file " + std::string{filename});
    }

    shaderContents.resize(shader.tellg(), '\0');
    shader.seekg(0);
    shader.read(shaderContents.data(), static_cast<std::streamsize>(shaderContents.size()));

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{vk::ShaderModuleCreateFlags{},
                                                      shaderContents.size(),
                                                      reinterpret_cast<u32*>(shaderContents.data())};

    shaderModule = device.createShaderModule(shaderModuleCreateInfo);

    SPDLOG_DEBUG("Loaded shader: {}", filename);
}

void VulkanCompute::createPipeline()
{
    std::vector binding = {
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute},
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute}};
    vk::DescriptorSetLayoutCreateInfo descriptorSetInfo{vk::DescriptorSetLayoutCreateFlags{}, binding};
    descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetInfo);

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{}, descriptorSetLayout};
    pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);
    pipelineCache  = device.createPipelineCache({});

    vk::PipelineShaderStageCreateInfo shaderStageInfo{vk::PipelineShaderStageCreateFlags{},
                                                      vk::ShaderStageFlagBits::eCompute,
                                                      shaderModule,
                                                      "main"};
    vk::ComputePipelineCreateInfo computePipelineInfo{vk::PipelineCreateFlags{}, shaderStageInfo, pipelineLayout};

    computePipeline = device.createComputePipeline(pipelineCache, computePipelineInfo).value;

    vk::DescriptorPoolSize descriptorPoolSize{vk::DescriptorType::eStorageBuffer, 2};
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{vk::DescriptorPoolCreateFlags{}, 1, descriptorPoolSize};
    descriptorPool = device.createDescriptorPool(descriptorPoolCreateInfo);

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{descriptorPool, descriptorSetLayout};
    auto descriptorSets = device.allocateDescriptorSets(descriptorSetAllocateInfo);
    descriptorSet       = descriptorSets.front();

    vk::DescriptorBufferInfo inputBufferInfo{inputBuffer, 0, inputMemorySize};
    vk::DescriptorBufferInfo outputBufferInfo{outputBuffer, 0, outputMemorySize};

    std::vector writeDescriptorSets = {
        vk::WriteDescriptorSet{descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &inputBufferInfo},
        vk::WriteDescriptorSet{descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &outputBufferInfo}};
    device.updateDescriptorSets(writeDescriptorSets, {});

    vk::CommandPoolCreateFlags commandPoolCreateFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    vk::CommandPoolCreateInfo commandPoolCreateInfo{commandPoolCreateFlags, queueIndex};
    commandPool = device.createCommandPool(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{commandPool, vk::CommandBufferLevel::ePrimary, 1};
    auto commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);
    commandBuffer       = commandBuffers.front();
}

u64 VulkanCompute::execute()
{
    SPDLOG_DEBUG("Submitting execution to GPU");

    #if defined(DebugRenderdoc)
    if (renderdocApi != nullptr)
    {
        SPDLOG_INFO("RenderDoc API: start frame");
        renderdocApi->StartFrameCapture(nullptr, nullptr);
    }
    #endif

    auto timer = Timers::create("Compute shader execution");

    vk::CommandBufferBeginInfo commandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(commandBufferBeginInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, descriptorSet, {});
    commandBuffer.dispatch(jobsX, jobsY, jobsZ);
    commandBuffer.end();

    try
    {
        vk::SubmitInfo submitInfo{{}, {}, commandBuffer};
        queue.submit(submitInfo, fence);
        if (device.waitForFences(fence, true, 10'000'000'000) == vk::Result::eTimeout)
        {
            SPDLOG_WARN("GPU timeout (calculation took over 10 seconds)");
        }
    }
    catch (const vk::Error& e)
    {
        SPDLOG_ERROR("Vulkan error during execution: {}", e.what());
        #if defined(DebugRenderdoc)
        if (renderdocApi != nullptr)
        {
            SPDLOG_INFO("RenderDoc API: end frame");
            renderdocApi->EndFrameCapture(nullptr, nullptr);
        }
        #endif
        throw;
    }

    commandBuffer.reset(vk::CommandBufferResetFlags{});
    device.resetFences(fence);

    #if defined(DebugRenderdoc)
    if (renderdocApi != nullptr)
    {
        SPDLOG_INFO("RenderDoc API: end frame");
        renderdocApi->EndFrameCapture(nullptr, nullptr);
    }
    #endif

    return timer.finish();
}

void VulkanCompute::download(u8* outputData)
{
    SPDLOG_INFO("Downloading data from GPU memory");
    auto timer      = Timers::create("Data download");
    auto outputView = static_cast<u8*>(device.mapMemory(outputMemory, 0, outputMemorySize));
    std::copy(outputView, outputView + outputMemorySize, outputData);
    SPDLOG_DEBUG("Download finished, cleaning memory");
    std::fill(outputView, outputView + outputMemorySize, 0);
    SPDLOG_DEBUG("Finished");
    device.unmapMemory(outputMemory);
}
