#include "slpch.h"

#include "Sleet/Renderer/ShaderData.h"

namespace Sleet {

	VertexInputAttribute::VertexInputAttribute(std::string name, ShaderDataType type, uint32_t location) :
		Name(name), Type(type), Location(location), Size(Utils::ShaderDataTypeSize(type)), Offset(0)
	{

	}

	bool operator==(const VertexInputAttribute& lhs, const VertexInputAttribute& rhs)
	{
		return (lhs.Type == rhs.Type);
	}

	bool operator!=(const VertexInputAttribute& lhs, const VertexInputAttribute& rhs)
	{
		return (lhs.Type != rhs.Type);
	}

	VertexInput::VertexInput(std::initializer_list<VertexInputAttribute> attributes) :
		m_Attributes(attributes), m_Binding(0)
	{
		CalculateOffsetAndStride();
	}

	void VertexInput::CalculateOffsetAndStride()
	{
		m_Stride = 0;
		uint32_t offset = 0;
		for (VertexInputAttribute& attribute : m_Attributes)
		{
			attribute.Offset = offset;
			offset += attribute.Size;
			m_Stride += attribute.Size;
		}
	}

	void VertexInput::Log()
	{
		SL_TRACE("Vertex input attributes, binding {} stride {}:", m_Binding, m_Stride);
		for (VertexInputAttribute& attribute : m_Attributes)
		{
			SL_TRACE("Name: {:<16} Type: {}\t Location: {}\t Offset: {}",
				attribute.Name, Utils::ShaderDataTypeToString(attribute.Type),
				attribute.Location, attribute.Offset);
		}
	}

	bool operator==(const VertexInput& lhs, const VertexInput& rhs)
	{
		size_t size = lhs.m_Attributes.size();
		if (size != rhs.m_Attributes.size())
		{
			return false;
		}

		for (int i = 0; i < size; i++)
		{
			if (lhs.m_Attributes[i] != rhs.m_Attributes[i])
			{
				return false;
			}
		}
		return true;
	}

	ShaderDescriptor::ShaderDescriptor() :
		Set(0), Binding(0), ShaderStageBits(0), Type(ShaderDescriptorType::None), Count(0)
	{

	}

	ShaderDescriptor::ShaderDescriptor(uint32_t set, uint32_t binding, ShaderStage stage, ShaderDescriptorType type, uint32_t count) :
		Set(set), Binding(binding), ShaderStageBits(stage), Type(type), Count(count)
	{

	}

	bool operator==(const ShaderDescriptor& lhs, const ShaderDescriptor& rhs)
	{
		return(
			(lhs.Set == rhs.Set) &&
			(lhs.Binding == rhs.Binding) &&
			(lhs.ShaderStageBits == rhs.ShaderStageBits) &&
			(lhs.Type == rhs.Type));
	}

	bool operator!=(const ShaderDescriptor& lhs, const ShaderDescriptor& rhs)
	{
		if (lhs.Set != rhs.Set) return true;
		if (lhs.Binding != rhs.Binding) return true;
		if (lhs.ShaderStageBits != rhs.ShaderStageBits) return true;
		if (lhs.Type != rhs.Type) return true;
		return false;
	}

	PushBlock::PushBlock() :
		ShaderStageBits(0), Offset(0), Size(0)
	{

	}

	PushBlock::PushBlock(ShaderStage stage, uint32_t offset, uint32_t size) :
		ShaderStageBits(stage), Offset(offset), Size(size)
	{

	}

}