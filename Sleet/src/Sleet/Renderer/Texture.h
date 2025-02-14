#pragma once

#include "Sleet/Core/Base.h"

#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {

	class Texture
	{
	public:
		~Texture() = default;

		virtual void Write(Ref<DescriptorSet> descriptorSet, uint32_t binding) = 0;

		static Ref<Texture> Create(const std::string filepath);
		static Ref<Texture> Create(uint32_t width, uint32_t height, void* data);
	private:

	};

}