#pragma once

#include "Sleet/Core/Base.h"

#include "Sleet/Renderer/ShaderData.h"

namespace Sleet {

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		static Ref<VertexBuffer> Create(void* data, uint32_t size);

		virtual const VertexInput& GetInput() const = 0;
		virtual void SetData(void* data) = 0;
	private:

	};

}