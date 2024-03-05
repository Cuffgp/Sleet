#pragma once

#include "Sleet/Core/Base.h"

#include <map>

namespace Sleet {

	enum class ImageFormat
	{
		None = 0,
		RGBA8U,
		RGBA32F,

		D32FS8U
	};

	enum class ShaderDescriptorType
	{
		None, Uniform, ImageSampler
	};

	enum class ShaderDataType
	{
		None, Float, Float2, Float3, Float4
	};

	enum class Usage
	{
		Static, Dynamic
	};

	enum ShaderStage : uint32_t
	{
		Vertex = BIT(1), Fragment = BIT(2), All = BIT(3)
	};

	struct VertexInputAttribute
	{
		VertexInputAttribute(std::string name, ShaderDataType type, uint32_t location);

		friend bool operator==(const VertexInputAttribute& lhs, const VertexInputAttribute& rhs);
		friend bool operator!=(const VertexInputAttribute& lhs, const VertexInputAttribute& rhs);

		std::string Name;
		ShaderDataType Type;
		uint32_t Location;
		uint32_t Size;
		uint32_t Offset;
	};

	class VertexInput
	{
	public:
		VertexInput() {}
		VertexInput(std::initializer_list<VertexInputAttribute> attributes);

		uint32_t GetBinding() { return m_Binding; }
		uint32_t GetStride() { return m_Stride; }
		const std::vector<VertexInputAttribute>& GetAttributes() { return m_Attributes; }

		std::vector<VertexInputAttribute>::iterator begin() { return m_Attributes.begin(); }
		std::vector<VertexInputAttribute>::iterator end() { return m_Attributes.end(); }
		std::vector<VertexInputAttribute>::const_iterator begin() const { return m_Attributes.begin(); }
		std::vector<VertexInputAttribute>::const_iterator end() const { return m_Attributes.end(); }

		void PushAttribute(VertexInputAttribute attribute) { m_Attributes.push_back(attribute); }
		void CalculateOffsetAndStride();
		void Log();

		friend bool operator==(const VertexInput& lhs, const VertexInput& rhs);

	private:
		uint32_t m_Binding = 0;
		uint32_t m_Stride = 0;
		std::vector<VertexInputAttribute> m_Attributes;
	};

	struct ShaderDescriptor
	{
		ShaderDescriptor();
		ShaderDescriptor(uint32_t set, uint32_t binding, ShaderStage stage, ShaderDescriptorType type, uint32_t count = 0);

		friend bool operator==(const ShaderDescriptor& lhs, const ShaderDescriptor& rhs);
		friend bool operator!=(const ShaderDescriptor& lhs, const ShaderDescriptor& rhs);

		uint32_t Set;
		uint32_t Binding;
		uint32_t ShaderStageBits;
		ShaderDescriptorType Type;
		uint32_t Count;
	};

	using DescriptorMap = std::map<uint32_t, ShaderDescriptor>;
	using DescriptorSetMap = std::map<uint32_t, DescriptorMap>;

	struct PushBlock
	{
		PushBlock();
		PushBlock(ShaderStage stage, uint32_t offset, uint32_t size);

		uint32_t ShaderStageBits;
		uint32_t Offset;
		uint32_t Size;
	};

	using PushMap = std::map<uint32_t, PushBlock>;

	namespace Utils {

		static std::string ShaderDataTypeToString(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::None:   return "None";
			case ShaderDataType::Float:  return "Float";
			case ShaderDataType::Float2: return "Float2";
			case ShaderDataType::Float3: return "Float3";
			case ShaderDataType::Float4: return "Float4";
			}
			SL_ASSERT(false, "Unknown shader data type");
			return "Unknown";
		}

		static uint32_t ShaderDataTypeSize(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::None:   return 0;
			case ShaderDataType::Float:  return 4;
			case ShaderDataType::Float2: return 8;
			case ShaderDataType::Float3: return 12;
			case ShaderDataType::Float4: return 16;
			}
			SL_ASSERT(false, "Unknown ShaderDataType");
			return 0;
		}

		static std::string ShaderDescriptorTypeToString(ShaderDescriptorType type)
		{
			switch (type)
			{
			case ShaderDescriptorType::None:   return "None";
			case ShaderDescriptorType::Uniform:  return "Uniform";
			case ShaderDescriptorType::ImageSampler:  return "ImageSampler";
			}
			SL_ASSERT(false, "Unknown shader data type");
			return "Unknown";
		}

		static std::string ShaderStageToString(uint32_t stage)
		{
			std::string result = "";
			if (stage & ShaderStage::Vertex) result += "Vertex ";
			if (stage & ShaderStage::Fragment) result += "Fragment ";

			return result;
		}
	}

}