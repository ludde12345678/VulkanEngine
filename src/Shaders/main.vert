#version 460
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

void main()
{
    vec2 positions[3] = vec2[](
        vec2(0.0, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, 0.5)
    );

    gl_Position = vec4(pos, 1.0);
    fragColor = color;
}