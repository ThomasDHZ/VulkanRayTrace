#include "RayTraceModel.h"
#include <chrono>

RayTraceModel::RayTraceModel()
{
}

RayTraceModel::RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath)
{
	Assimp::Importer ModelImporter;
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));

	const aiScene* Scene = ModelImporter.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "Error loading model: " << ModelImporter.GetErrorString() << std::endl;
		return;
	}

	LoadMesh(device, physicalDevice, FilePath, Scene->mRootNode, Scene);
}

RayTraceModel::~RayTraceModel()
{
}

void RayTraceModel::LoadMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath, aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		Mesh ModelMesh{};
		ModelMesh.vertices = LoadVertices(mesh);
		ModelMesh.indices = LoadIndices(mesh);
		ModelMesh.Transform = glm::mat4(1.0f);

		ModelMesh.VertexCount = ModelMesh.vertices.size();
		ModelMesh.IndexCount = ModelMesh.indices.size();
		ModelMesh.TriangleCount = static_cast<uint32_t>(ModelMesh.indices.size()) / 3;

		ModelMesh.VertexBuffer.CreateBuffer(device, physicalDevice, ModelMesh.VertexCount * sizeof(RTVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.vertices.data());
		ModelMesh.IndexBuffer.CreateBuffer(device, physicalDevice, ModelMesh.IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.indices.data());
		ModelMesh.TransformBuffer.CreateBuffer(device, physicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelMesh.Transform);

		ModelMesh.VertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.VertexBuffer.Buffer);
		ModelMesh.IndexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.IndexBuffer.Buffer);
		ModelMesh.TransformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.TransformBuffer.Buffer);

		ModelMesh.UniformBuffer.CreateBuffer(device, physicalDevice, sizeof(UniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelMesh.ubo);
		MeshList.emplace_back(ModelMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(device, physicalDevice, FilePath, node->mChildren[i], scene);
	}
}

std::vector<RTVertex> RayTraceModel::LoadVertices(aiMesh* mesh)
{
	std::vector<RTVertex> VertexList;

	for (int x = 0; x < mesh->mNumVertices; x++)
	{
		RTVertex vertex;
		vertex.Position = glm::vec3{ mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z };
		vertex.Normal = glm::vec3{ mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z };

		if (mesh->mTangents)
		{
			vertex.Tangant = glm::vec4{ mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z, 0.0f };
			vertex.BiTangant = glm::vec4{ mesh->mBitangents[x].x, mesh->mBitangents[x].y, mesh->mBitangents[x].z, 0.0f };
		}
		else
		{
			vertex.Tangant = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
			vertex.BiTangant = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.TexureCoord = glm::vec2{ mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y };
		}
		else
		{
			vertex.TexureCoord = glm::vec2{ 0.0f, 0.0f };
		}

		vertex.BoneID = glm::vec4(0.0f);
		vertex.BoneWeights = glm::vec4(0.0f);

		VertexList.emplace_back(vertex);
	}

	return VertexList;
}

std::vector<uint32_t> RayTraceModel::LoadIndices(aiMesh* mesh)
{
	std::vector<uint32_t> IndexList;

	for (int x = 0; x < mesh->mNumFaces; x++)
	{
		aiFace Faces = mesh->mFaces[x];
		for (int y = 0; y < Faces.mNumIndices; y++)
		{
			IndexList.emplace_back(Faces.mIndices[y]);
		}
	}

	return IndexList;
}

uint64_t RayTraceModel::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
{
	VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
	bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferDeviceAI.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}

void RayTraceModel::Update(VkDevice& device, std::shared_ptr<PerspectiveCamera> camera)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto  currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	for (auto& mesh : MeshList)
	{
		mesh.ubo.projInverse = glm::inverse(camera->GetProjectionMatrix());
		mesh.ubo.viewInverse = glm::inverse(camera->GetViewMatrix());
		mesh.ubo.modelInverse = glm::inverse(glm::mat4(1.0f));
		mesh.ubo.lightPos = glm::vec4(cos(glm::radians(time * 360.0f)) * 40.0f, -50.0f + sin(glm::radians(time * 360.0f)) * 20.0f, 25.0f + sin(glm::radians(time * 360.0f)) * 5.0f, 0.0f);
		mesh.ubo.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
		mesh.ubo.vertexSize = sizeof(RTVertex);
		mesh.UniformBuffer.CopyBufferToMemory(device, &mesh.ubo, sizeof(UniformData));
	}
}
