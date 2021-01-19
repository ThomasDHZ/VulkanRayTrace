#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include "PerspectiveCamera.h"
#include "VulkanBuffer.h"

struct UniformData {
	glm::mat4 viewInverse;
	glm::mat4 projInverse;
	glm::mat4 modelInverse;
	glm::vec4 lightPos;
	glm::vec4 viewPos;
	int vertexSize;
};

struct RTVertex
{
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Normal = glm::vec3(0.0f);
	glm::vec2 TexureCoord = glm::vec2(0.0f);
	glm::vec4 Tangant = glm::vec4(0.0f);
	glm::vec4 BiTangant = glm::vec4(0.0f);
	glm::vec4 Color = glm::vec4(0.0f);
	glm::ivec4 BoneID = glm::ivec4(0);
	glm::vec4 BoneWeights = glm::vec4(0.0f);
};

struct MeshDetails
{
	std::vector<RTVertex> vertices;
	std::vector<uint32_t> indices;
};

struct Mesh
{
	std::vector<RTVertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 Transform;
	UniformData ubo;

	VulkanBuffer IndexBuffer;
	VulkanBuffer VertexBuffer;
	VulkanBuffer TransformBuffer;
	VulkanBuffer UniformBuffer;

	uint32_t TriangleCount;
	uint32_t VertexCount;
	uint32_t IndexCount;

	VkDeviceOrHostAddressConstKHR VertexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR IndexBufferDeviceAddress{};
	VkDeviceOrHostAddressConstKHR TransformBufferDeviceAddress{};
};

class RayTraceModel
{
private:
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;

	void LoadMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath, aiNode* node, const aiScene* scene);
	std::vector<RTVertex> LoadVertices(aiMesh* mesh);
	std::vector<uint32_t> LoadIndices(aiMesh* mesh);
	uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);

public:
	std::vector<Mesh> MeshList;

	RayTraceModel();
	RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath);
	~RayTraceModel();

	void Update(VkDevice& device, std::shared_ptr<PerspectiveCamera> camera);
};

