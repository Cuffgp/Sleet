#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPosition;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 View;
	mat4 Projection;
	vec4 AmbientLight;
	vec4 LightPosition;
	vec4 LightColour;
} Scene;

layout( push_constant ) uniform constants
{
	mat4 Transform;
} PushConstants;

void main() 
{
	gl_Position = Scene.Projection * Scene.View * PushConstants.Transform * vec4(inPosition, 1.0f);
	fragPosition = vec3(PushConstants.Transform * vec4(inPosition, 1.0));

	fragTexCoord = inUv;
	fragNormal = inNormal;
}

#type fragment
#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 View;
	mat4 Projection;
	vec4 AmbientLight;
	vec4 LightPosition;
	vec4 LightColour;
} Scene;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() 
{

	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(Scene.LightPosition.xyz - fragPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * Scene.LightColour.xyz;
  
	vec4 combinedLights = vec4(Scene.AmbientLight.xyz + diffuse, 1.0);
	vec4 texColor = texture(texSampler, fragTexCoord);
	outColor = texColor * combinedLights;

}