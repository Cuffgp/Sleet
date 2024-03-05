#pragma once

#include "Sleet/Renderer/ShaderData.h"

namespace Sleet {

	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		virtual DescriptorMap& GetDescriptorMap() = 0;

		static Ref<DescriptorSet> Create(std::vector<ShaderDescriptor> descriptors);
		static Ref<DescriptorSet> Create(DescriptorMap descriptorMap);

	private:
	};

}