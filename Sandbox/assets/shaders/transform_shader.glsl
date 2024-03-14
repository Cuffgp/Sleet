#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout( push_constant ) uniform constants
{
    mat4 Transform;
} PushConstants;

void main() {
    gl_Position = PushConstants.Transform * vec4(inPosition, 1.0f);
    fragColor = vec3(1.0, 1.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}