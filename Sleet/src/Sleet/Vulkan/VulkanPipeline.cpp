#include "slpch.h"

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"

#include "Sleet/Vulkan/VulkanSwapchain.h"

namespace Sleet {

	VulkanPipeline::VulkanPipeline(std::string filepath):
		m_Filepath(filepath)
	{
		m_Shader = CreateScope<VulkanShader>(filepath);
		m_Input = m_Shader->GetVertexInput();
		m_DescriptorSetMap = m_Shader->GetDescriptorSetMap();

		CreateDescriptorSetLayout(m_DescriptorSetMap);
		CreatePipelineLayout();
		CreatePipeline();
	}

	VulkanPipeline::VulkanPipeline(std::string filepath, DescriptorSetMap descriptorSetMap):
		m_Filepath(filepath)
	{
		m_Shader = CreateScope<VulkanShader>(filepath);
		m_Input = m_Shader->GetVertexInput();
		m_DescriptorSetMap = m_Shader->GetDescriptorSetMap();

		CreateDescriptorSetLayout(m_DescriptorSetMap);
		CreatePipelineLayout();
		CreatePipeline();
	}

	VulkanPipeline::VulkanPipeline(std::string vertPath, std::string fragPath)
	{
		m_Shader = CreateScope<VulkanShader>(vertPath, fragPath);
		m_Input = m_Shader->GetVertexInput();
		m_DescriptorSetMap = m_Shader->GetDescriptorSetMap();

		CreateDescriptorSetLayout(m_DescriptorSetMap);
		CreatePipelineLayout();
		CreatePipeline();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto device = VulkanDevice::Get().Device();

		vkDeviceWaitIdle(device);
		for (auto descriptorSetlayout : m_DescriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(device, descriptorSetlayout, nullptr);
		}
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyPipeline(device, m_Pipeline, nullptr);
	}

	void VulkanPipeline::Recreate()
	{
		// Should allow for hot recreation of a pipeline
		// (but dont change the vertex input or descriptors)
		auto device = VulkanDevice::Get().Device();
		vkDeviceWaitIdle(device);
		m_Shader.reset();
		vkDestroyPipeline(device, m_Pipeline, nullptr);

		m_Shader = CreateScope<VulkanShader>(m_Filepath, true);
		CreatePipeline();
	}

	void VulkanPipeline::CreateDescriptorSetLayout(DescriptorSetMap descriptorSetMap)
	{
		auto device = VulkanDevice::Get().Device();

		for (const auto& setMap : descriptorSetMap)
		{
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

			for (const auto& bindingMap : setMap.second)
			{
				VkDescriptorSetLayoutBinding uboLayoutBinding{};
				uboLayoutBinding.binding = bindingMap.second.Binding;
				uboLayoutBinding.descriptorType = Utils::ShaderAttachmentToVulkan(bindingMap.second.Type);
				uboLayoutBinding.descriptorCount = 1;
				uboLayoutBinding.stageFlags = Utils::ShaderStageToVulkan(bindingMap.second.ShaderStageBits);
				uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

				descriptorSetLayoutBindings.push_back(uboLayoutBinding);
			}

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
			layoutInfo.pBindings = descriptorSetLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetlayout;
			if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetlayout) != VK_SUCCESS)
			{
				SL_ERROR("failed to create descriptor set layout!");
			}

			m_DescriptorSetLayouts.push_back(descriptorSetlayout);
		}
	}

	void VulkanPipeline::CreatePipelineLayout()
	{
		auto device = VulkanDevice::Get().Device();

		std::vector<VkPushConstantRange> pushConstantRange;

		for (const auto& push : m_Shader->GetPushMap())
		{
			//SL_INFO("Push Shader Stage {}", push.second.ShaderStageBits);
			//SL_INFO("Vulkan flag bits {}", Utils::ShaderStageToVulkan(push.second.ShaderStageBits));

			VkPushConstantRange pushConstant{};
			pushConstant.stageFlags = Utils::ShaderStageToVulkan(push.second.ShaderStageBits);
			pushConstant.offset = push.second.Offset;
			pushConstant.size = push.second.Size;

			pushConstantRange.push_back(pushConstant);
		}

		SL_INFO("Push Size, {}", m_Shader->GetPushMap().size());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = m_DescriptorSetLayouts.size(); // Optional
		pipelineLayoutInfo.pSetLayouts = m_DescriptorSetLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantRange.size(); // Optional
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRange.data(); // Optional

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
		{
			SL_ERROR("failed to create pipeline layout!");
		}
	}

	void VulkanPipeline::CreatePipeline()
	{
		auto device = VulkanDevice::Get().Device();

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_Shader->GetVertModule();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_Shader->GetFragModule();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<VkDynamicState> dynamicStates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = m_Input.GetBinding();
		bindingDescription.stride = m_Input.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		for (auto& attribute : m_Input.GetAttributes())
		{
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = m_Input.GetBinding();
			attributeDescription.location = attribute.Location;
			attributeDescription.format = Utils::ShaderDataTypeToVulkan(attribute.Type);
			attributeDescription.offset = attribute.Offset;

			attributeDescriptions.push_back(attributeDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = nullptr;
		viewportState.scissorCount = 1;
		viewportState.pScissors = nullptr;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_TRUE;
		depthStencil.minDepthBounds = 0.0f;  // Optional
		depthStencil.maxDepthBounds = 1.0f;  // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};  // Optional
		depthStencil.back = {};   // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachmentFormats = &VulkanSwapchain::s_SwapchainImageFormat;
		renderingInfo.depthAttachmentFormat = VulkanSwapchain::s_SwapchainDepthFormat;
		renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = nullptr;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		pipelineInfo.pNext = &renderingInfo;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) 
		{
			SL_ERROR("Failed to create graphics pipeline!");
		}
	}

}