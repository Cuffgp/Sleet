#pragma once

#include "Sleet/Core/Camera.h"

#include "vulkan/vulkan.h"

namespace Sleet {

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera &camera;
	};

}