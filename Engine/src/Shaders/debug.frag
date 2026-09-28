#version 460
#extension GL_KHR_vulkan_glsl : enable
#include "./common/Light.glsl"
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform FrameUBO
{
	uint FrameCount;
	float deltaTime;
	float totalTime;
	uint lightCount;
} Frame;

layout(set = 0, binding = 2) readonly buffer LightBuffer
{
    Light lights[];
};  

layout(set = 2, binding = 1) uniform sampler2D shadowMap[1];

void main()
{
    float depth = texture(shadowMap[0], uv).r;
    outColor = vec4(vec3(depth), 1.0);
}