#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanBuffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Sleet {

	class VulkanModel 
	{
	public:
		struct Vertex 
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color &&
					normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			void loadModel(const std::string &filepath);
		};

		VulkanModel(VulkanDevice& device, const VulkanModel::Builder &builder);
		~VulkanModel();

		VulkanModel(const VulkanModel&) = delete;
		VulkanModel& operator=(const VulkanModel&) = delete;

		static Scope<VulkanModel> createModelFromFile(VulkanDevice &device, const std::string &filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t> &indices);

		VulkanDevice& device;

		Scope<VulkanBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		Scope<VulkanBuffer> indexBuffer;
		uint32_t indexCount;
	};


}