#include "slpch.h"

#include "Sleet/Renderer/DescriptorSet.h"

#include "Sleet/Vulkan/VulkanDescriptorSet.h"

namespace Sleet {

	Ref<DescriptorSet> DescriptorSet::Create(std::vector<ShaderDescriptor> descriptors)
	{
		return CreateRef<VulkanDescriptorSet>(descriptors);
	}

	Ref<DescriptorSet> DescriptorSet::Create(DescriptorMap descriptorMap)
	{
		return CreateRef<VulkanDescriptorSet>(descriptorMap); 
	}


}