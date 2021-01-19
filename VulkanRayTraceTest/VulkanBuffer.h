#pragma once
#include <vulkan/vulkan.h>

class VulkanBuffer
{
private:

	VkDescriptorBufferInfo BufferDescriptor;

	uint32_t GetMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

protected:



public:
	VkBuffer Buffer = VK_NULL_HANDLE;
	VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
	VkDeviceSize BufferSize = VK_NULL_HANDLE;
	uint64_t BufferDeviceAddress = VK_NULL_HANDLE;
	VkAccelerationStructureKHR BufferHandle = VK_NULL_HANDLE;

	VulkanBuffer();
	~VulkanBuffer();
	VkResult CreateBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData = nullptr);
	void CopyBuffer(VkDevice& device, VkQueue& GraphicsQueue, VkCommandPool& renderCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CopyBufferToMemory(VkDevice& device, void* DataToCopy, VkDeviceSize BufferSize);
	void DestoryBuffer(VkDevice& device);
	VkCommandBuffer BeginSingleTimeCommand(VkDevice& device, VkCommandPool& renderCommandPool);
	void EndSingleTimeCommand(VkDevice& device, VkQueue& GraphicsQueue, VkCommandPool& renderCommandPool, VkCommandBuffer commandBuffer);
	
	VkBuffer GetBuffer() { return Buffer; }
	VkDeviceMemory GetBufferMemory() { return BufferMemory; }
	VkDeviceSize GetBufferSize() { return BufferSize; }
};

