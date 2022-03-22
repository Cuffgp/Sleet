#pragma once

#include <string>
#include <vector>

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	struct PipelineConfigInfo {};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(
			VulkanDevice& device, 
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);
		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		VulkanDevice& vulkanDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertexShader;
		VkShaderModule fragmentShader;

	};

}