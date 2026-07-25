#version 460
#extension GL_EXT_vulkan_glsl_definitions : enable

// outputs
layout(location = 0) out vec4 frag_color;

void main()
{
frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}