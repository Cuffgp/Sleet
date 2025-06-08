

#type vertex
#version 450

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 fragColor;

layout( push_constant ) uniform constants
{
	mat4 Transform;
} PushConstants;

void main() 
{
    gl_Position = PushConstants.Transform * vec4(inPosition, 0.0, 1.0);
    fragColor = vec3(0.6, 0.3, 0.3);
}

#type fragment
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(fragColor, 1.0);
}