#type vertex
#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

layout( push_constant ) uniform constants
{
    mat4 Transform;
} PushConstants;

void main() {
    gl_Position = PushConstants.Transform * vec4(inPosition, 1.0f);
}

#type fragment
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 colors[6];
} ubo;

void main() {

    outColor = ubo.colors[gl_PrimitiveID  >> 1];
    //outColor = vec4(fragColor, 1.0);
}