#pragma once

#include "Sleet/Renderer/DescriptorSet.h"

#include "Sleet/Vulkan/VulkanDevice.h"

namespace Sleet {

	class VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(std::vector<ShaderDescriptor> descriptors);
		VulkanDescriptorSet(DescriptorMap descriptorMap);
		~VulkanDescriptorSet();

		virtual DescriptorMap& GetDescriptorMap() override { return m_DescriptorMap; }

		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
	private:

		void CreateDescriptorSetLayout();
		void CreateDescriptorSet();

		DescriptorMap m_DescriptorMap;
		VkDescriptorSetLayout m_Layout;
		VkDescriptorSet m_DescriptorSet;

	};

}