#pragma once

#include "Sleet/Core/Base.h"
#include "Sleet/Renderer/ShaderData.h"

namespace Sleet {

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		static Ref<Pipeline> Create(std::string filePath);
		static Ref<Pipeline> Create(std::string filePath, DescriptorSetMap descriptorSetMap);
		static Ref<Pipeline> Create(std::string vertPath, std::string fragPath);

		virtual void Recreate() = 0;
	private:

	};

}