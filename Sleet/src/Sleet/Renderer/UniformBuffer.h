#pragma once

#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {

	class UniformBuffer
	{
	public:

		virtual ~UniformBuffer() = default;

		static Ref<UniformBuffer> Create(uint32_t size);

		virtual void SetData(void* data) = 0;

		virtual void Write(Ref<DescriptorSet> descriptorSet) = 0;

	private:

	};

}