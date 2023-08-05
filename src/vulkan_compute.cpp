#include "vulkan_compute.hpp"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>

VulkanCompute::VulkanCompute(
    usize inputMemorySize, usize outputMemorySize, std::string_view shader, u32 jobsX, u32 jobsY, u32 jobsZ)
    : inputMemorySize{inputMemorySize}
    , outputMemorySize{outputMemorySize}
{
    SPDLOG_INFO("Creating Vulkan compute helper");
    SPDLOG_DEBUG("Requested memory: input buffer {} bytes, output buffer {} bytes", inputMemorySize, outputMemorySize);
    SPDLOG_DEBUG("Shader file: {}", shader);

    TIMER_START;

    vk::InstanceCreateInfo instanceInfo{vk::InstanceCreateFlags{}, {}, {}, {}};
    instance = vk::createInstance(instanceInfo);

    createDevice();
    allocateMemory();
    loadShader(shader);
    createPipeline();
    createCommandBuffer(jobsX, jobsY, jobsZ);

    queue = device.getQueue(queueIndex, 0);
    fence = device.createFence(vk::FenceCreateInfo{});

    SPDLOG_DEBUG("Vulkan setup finished");

    TIMER_END("Vulkan setup");
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

void VulkanCompute::createDevice()
{
    auto devices = instance.enumeratePhysicalDevices();
    SPDLOG_DEBUG("Available devices:");
    for (const auto& dev : devices)
    {
        auto properties = dev.getProperties();
        SPDLOG_DEBUG("    {} - {}, API version {}.{}, compute shared memory {} KB",
                     vk::to_string(properties.deviceType),
                     std::string_view(properties.deviceName),
                     VK_VERSION_MAJOR(properties.apiVersion),
                     VK_VERSION_MINOR(properties.apiVersion),
                     properties.limits.maxComputeSharedMemorySize / 1024);
    }

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
        throw std::runtime_error("No gpu");
    }

    physicalDevice = *deviceIt;
    SPDLOG_INFO("Using physical device {}", std::string_view(physicalDevice.getProperties().deviceName));

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
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
    device                = physicalDevice.createDevice(deviceCreateInfo);
}

void VulkanCompute::allocateMemory()
{
    SPDLOG_DEBUG("Allocating GPU memory");

    vk::BufferCreateInfo inputBufferCreateInfo{vk::BufferCreateFlags{},
                                               inputMemorySize,
                                               vk::BufferUsageFlagBits::eStorageBuffer,
                                               vk::SharingMode::eExclusive,
                                               1,
                                               &queueIndex};

    vk::BufferCreateInfo outputBufferCreateInfo{vk::BufferCreateFlags{},
                                                outputMemorySize,
                                                vk::BufferUsageFlagBits::eStorageBuffer,
                                                vk::SharingMode::eExclusive,
                                                1,
                                                &queueIndex};

    inputBuffer  = device.createBuffer(inputBufferCreateInfo);
    outputBuffer = device.createBuffer(outputBufferCreateInfo);

    auto inputMemoryReq  = device.getBufferMemoryRequirements(inputBuffer);
    auto outputMemoryReq = device.getBufferMemoryRequirements(outputBuffer);

    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
    auto memoryTypeIt                                   = std::find_if(memoryProperties.memoryTypes.begin(),
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
    std::ifstream shader{filename, std::ios::binary | std::ios::ate};

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
}

void VulkanCompute::createCommandBuffer(u32 jobsX, u32 jobsY, u32 jobsZ)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo{{}, queueIndex};
    commandPool = device.createCommandPool(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{commandPool, vk::CommandBufferLevel::ePrimary, 1};

    auto commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);
    commandBuffer       = commandBuffers.front();

    vk::CommandBufferBeginInfo commandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    commandBuffer.begin(commandBufferBeginInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, descriptorSet, {});
    commandBuffer.dispatch(jobsX, jobsY, jobsZ);
    commandBuffer.end();
}

void VulkanCompute::execute(u64 timeout)
{
    SPDLOG_INFO("Submitting execution to GPU");
    SPDLOG_DEBUG("Execution time limit is set to {} ms",
                 static_cast<float>(timeout / 1e3) / 1e3);
    TIMER_START;
    vk::SubmitInfo submitInfo{{}, {}, commandBuffer};
    queue.submit(submitInfo, fence);

    if (device.waitForFences(fence, true, timeout) == vk::Result::eTimeout)
    {
        SPDLOG_ERROR("Execution timed out");
        throw std::runtime_error("Timeout");
    }
    auto stop = std::chrono::system_clock::now();
    TIMER_END("Compute shader execution");
}
