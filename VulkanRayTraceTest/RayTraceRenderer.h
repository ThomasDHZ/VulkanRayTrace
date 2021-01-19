#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include "VulkanBuffer.h"
#include <vector>
#include "Buffer.h"
#include "RayTraceModel.h"
#include "PerspectiveCamera.h"
#include "Keyboard.h"
#include "Mouse.h"

struct RayTracingScratchBuffer
{
    uint64_t deviceAddress = 0;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct StorageImage {
    VkDeviceMemory memory;
    VkImage image;
    VkImageView view;
    VkFormat format;
};

struct AccelerationStructure {
    VkAccelerationStructureKHR handle;
    uint64_t deviceAddress = 0;
    VkDeviceMemory memory;
    VkBuffer buffer;
};

struct Texture
{
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
};

class RayTraceRenderer
{
private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t WIDTH;
    uint32_t HEIGHT;

    PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
    PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
    PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
    PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
    PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
    PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

    VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

    StorageImage storageImage;

    VkPipeline            RayTracePipeline;
    VkPipelineLayout      RayTracePipelineLayout;
    VkDescriptorSet       RTDescriptorSet;
    VkDescriptorSetLayout RayTraceDescriptorSetLayout;
    VkDescriptorPool      descriptorPool;

    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

public:

    std::shared_ptr<PerspectiveCamera> camera;
    Keyboard keyboard;
    Mouse mouse;

    Texture DiffuseMap;

    std::vector<AccelerationStructure> bottomLevelASList{};
    AccelerationStructure topLevelAS{};

    std::vector<VkShaderModule> shaderModules;
    std::vector<VkCommandBuffer> drawCmdBuffers;

    RayTraceRenderer();
    RayTraceRenderer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDescriptorPool descriptorPool, uint32_t WIDTH, uint32_t HEIGHT, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages);
    ~RayTraceRenderer();


    RayTraceModel model;
    void createBottomLevelAccelerationStructure(RayTraceModel& model);
    void createTopLevelAccelerationStructure(RayTraceModel& model);
    void createStorageImage();
    void createRayTracingPipeline();
    void createShaderBindingTable();
    void createDescriptorSets(RayTraceModel& model);
    void buildCommandBuffers(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages);
    void Resize(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height);

    void AcclerationCommandBuffer(VkAccelerationStructureBuildGeometryInfoKHR& VkAccelerationStructureBuildGeometryInfoKHR, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& accelerationStructureBuildRangeInfoKHR);
    void updateUniformBuffers(GLFWwindow* window);

    VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer* buffer, VkDeviceSize size, void* data);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void deleteScratchBuffer(RayTracingScratchBuffer& scratchBuffer);
    void createAccelerationStructure(AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo);
    RayTracingScratchBuffer createScratchBuffer(VkDeviceSize size);
    uint64_t getBufferDeviceAddress(VkBuffer buffer);
    uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties);
    VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin);
    VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin);
    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkImageSubresourceRange subresourceRange,
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);
    VkShaderModule loadShader(const char* fileName, VkDevice device);
    uint32_t alignedSize(uint32_t value, uint32_t alignment);

    VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix)
    {
        return VkTransformMatrixKHR
        {
            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
        };
    }


    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
    //Texture
    void createTextureImage(Texture& texture, const std::string Filepath);
    void createTextureImageView(Texture& texture);
    void createTextureSampler(Texture& texture);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};