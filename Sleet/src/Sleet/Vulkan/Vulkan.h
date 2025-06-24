#pragma once

#include "Sleet/Renderer/ShaderData.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include <vulkan/vk_enum_string_helper.h>

namespace Sleet {

	namespace Utils {

		static VkFormat ShaderDataTypeToVulkan(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			SL_ASSERT(false, "Unknown shader data type");
			return VK_FORMAT_UNDEFINED;
		}

		static VkFormat ImageFormatToVulkan(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA8U: return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32_SFLOAT;
			case ImageFormat::R32U: return VK_FORMAT_R32_UINT;

			case ImageFormat::D32FS8U: return VK_FORMAT_D32_SFLOAT_S8_UINT;
			}
			SL_ASSERT(false, "Unknown image format");
			return VK_FORMAT_UNDEFINED;
		}

		static VkShaderStageFlagBits ShaderStageToVulkan(uint32_t stageFlags)
		{
			VkShaderStageFlags flags = 0;

			if (stageFlags & ShaderStage::Vertex) flags |= VK_SHADER_STAGE_VERTEX_BIT;
			if (stageFlags & ShaderStage::Fragment) flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			if (stageFlags & ShaderStage::All) 
			{
				flags |= VK_SHADER_STAGE_VERTEX_BIT;
				flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			return VkShaderStageFlagBits(flags);
		}

		static VkDescriptorType ShaderAttachmentToVulkan(ShaderDescriptorType attachment)
		{
			switch (attachment)
			{
			case ShaderDescriptorType::Uniform: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case ShaderDescriptorType::ImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			}
			SL_ASSERT(false, "Unknown shader attachment");
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		}
	}
}