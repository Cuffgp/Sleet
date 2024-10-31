#pragma once

#include "Sleet/Core/Base.h"

#include "Sleet/Renderer/Texture.h"

namespace Sleet {

	class Material
	{
	public:

	public:
		std::string Name = "Unnamed";
		glm::vec3 Ambient = glm::vec3{ 1.0, 1.0, 1.0 };
		glm::vec3 Diffuse = glm::vec3{ 1.0, 1.0, 1.0 };
		glm::vec3 Specular = glm::vec3{ 1.0, 1.0, 1.0 };
		glm::vec3 Transmission = glm::vec3{ 1.0, 1.0, 1.0 };
		glm::vec3 Emission = glm::vec3{ 1.0, 1.0, 1.0};
		float Shinyness = 1.0f;
		Ref<Texture> AmbientTexture = nullptr;
	};

}