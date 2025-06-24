#include "slpch.h"

#include "Sleet/Renderer/RendererAPI.h"
#include "Sleet/Renderer/Pipeline.h"

#include "Sleet/Vulkan/VulkanPipeline.h"

namespace Sleet {

	Ref<Pipeline> Pipeline::Create(std::string filepath)
	{
		return CreateRef<VulkanPipeline>(filepath);
	}

	Ref<Pipeline> Create(std::string filepath, Ref<Framebuffer> framebuffer)
	{
		return CreateRef<VulkanPipeline>(filepath, framebuffer);
	}

	Ref<Pipeline> Pipeline::Create(std::string filepath, DescriptorSetMap descriptorSetMap)
	{
		return CreateRef<VulkanPipeline>(filepath, descriptorSetMap);
	}

	Ref<Pipeline> Pipeline::Create(std::string vertPath, std::string fragPath)
	{
		return CreateRef<VulkanPipeline>(vertPath, fragPath);
	}

}