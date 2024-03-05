#pragma once

#include "Sleet/Renderer/RendererAPI.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Core/Log.h"

extern Sleet::Application* Sleet::CreateApplication();

int main(int argc, char** argv)
{
	Sleet::Log::Init();

	Sleet::VulkanDevice::Init();

	auto app = Sleet::CreateApplication();
	app->Run();
	delete app;

	Sleet::VulkanDevice::Destroy();

}