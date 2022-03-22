#pragma once

#include <string>
#include <vector>

namespace Sleet {

	class VulkanPipeline
	{
	public:
		VulkanPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;


	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

	};

}