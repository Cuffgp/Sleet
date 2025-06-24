#pragma once

#include "Sleet/Renderer/ShaderData.h"
#include "Sleet/Renderer/DescriptorSet.h"

namespace Sleet {

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Write(Ref<DescriptorSet> descriptorSet, uint32_t binding, uint32_t index) = 0;

		static Ref<Framebuffer> Create(uint32_t width, uint32_t height);
		static Ref<Framebuffer> Create(uint32_t width, uint32_t height, std::vector<ImageFormat> attachments);
	private:

	};

}