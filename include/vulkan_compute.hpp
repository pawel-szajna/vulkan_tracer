#pragma once

#include <vulkan/vulkan.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <optional>
#include <string_view>
#include <spdlog/spdlog.h>

#include "basic_types.hpp"
#include "helpers.hpp"
#include "timers.hpp"

class VulkanCompute
{
public:

    VulkanCompute(usize inputMemorySize,
                  usize outputMemorySize,
                  std::string_view shader,
                  u32 jobsX,
                  u32 jobsY,
                  u32 jobsZ);
    ~VulkanCompute();

    /// @return Time it actually took to execute the load
    u64 execute(u64 timeout = 1000000000);

    void download(u8* outputData);

    template<typename T>
    void upload(const T& inputData)
    {
        SPDLOG_INFO("Uploading data to GPU memory");
        if (sizeof(T) != inputMemorySize)
        {
            SPDLOG_WARN("Sending {} bytes to {} bytes sized buffer",
                        sizeof(T), inputMemorySize);
        }
        auto timer = Timers::create("Data upload");
        auto memoryView = static_cast<T*>(device.mapMemory(inputMemory, 0, inputMemorySize));
        *memoryView = inputData;
        SPDLOG_DEBUG("Upload finished, unmapping memory");
        device.unmapMemory(inputMemory);
    }

private:

    void createDevice();
    void allocateMemory();
    void loadShader(std::string_view filename);
    void createPipeline();

    usize inputMemorySize;
    usize outputMemorySize;

    u32 queueIndex{};
    u32 jobsX, jobsY, jobsZ;

    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::DeviceMemory inputMemory;
    vk::DeviceMemory outputMemory;
    vk::ShaderModule shaderModule;
    vk::Buffer inputBuffer;
    vk::Buffer outputBuffer;
    vk::DescriptorSet descriptorSet;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    vk::PipelineLayout pipelineLayout;
    vk::PipelineCache pipelineCache;
    vk::Pipeline computePipeline;
    vk::CommandBuffer commandBuffer;
    vk::CommandPool commandPool;
    vk::Queue queue;
    vk::Fence fence;
};
