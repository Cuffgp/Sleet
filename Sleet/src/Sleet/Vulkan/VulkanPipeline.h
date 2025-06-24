#pragma once

#include "Sleet/Renderer/ShaderData.h"
#include "Sleet/Renderer/Pipeline.h"

#include "Sleet/Vulkan/VulkanShader.h"

#include "Vulkan.h"

namespace Sleet {

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(std::string filepath);
		VulkanPipeline(std::string filepath, Ref<Framebuffer> framebuffer);
		VulkanPipeline(std::string filepath, DescriptorSetMap descriptorSetMap);
		VulkanPipeline(std::string vertPath, std::string fragPath);
		~VulkanPipeline();

		VkPipeline GetPipeline() { return m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }

		void Recreate() override;

		VkPipelineRenderingCreateInfo m_PipelineRenderingInfo{};

	private:
		void CreateDescriptorSetLayout(DescriptorSetMap descriptorSetMap);
		void CreatePipelineLayout();
		void CreatePipeline();

	private:
		std::string m_Filepath;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		Scope<VulkanShader> m_Shader;
		VertexInput m_Input;
		DescriptorSetMap m_DescriptorSetMap;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		
	};

}