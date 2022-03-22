
#include <fstream>
#include <stdexcept>
#include <iostream>

#include "Sleet/Vulkan/VulkanPipeline.h"

namespace Sleet {

	VulkanPipeline::VulkanPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		createGraphicsPipeline(vertFilepath, fragFilepath);
	}

	VulkanPipeline::~VulkanPipeline()
	{

	}

	std::vector<char> VulkanPipeline::readFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	void VulkanPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);

		std::cout << "Vertex Shader file size: " << vertCode.size() << std::endl;
		std::cout << "Fragment Shader file size: " << fragCode.size() << std::endl;
	}

}