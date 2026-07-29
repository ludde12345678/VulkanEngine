#version 460


layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

layout(set = 0, binding = 0) uniform TimeUBO
{
    uint FrameCount;
} Time;

void main()
{

    float angle = float(Time.FrameCount) * 0.001; // rotation speed

    float c = cos(angle);
    float s = sin(angle);

    mat2 rotation = mat2(
        c, -s,
        s,  c
    );

    vec2 rotatedPos = rotation * pos.xy;

    gl_Position = vec4(rotatedPos, pos.z, 1.0);
    float t = float(Time.FrameCount % 3000) / 3000.0;

        fragColor = vec4(
        t,              // red changes
        1.0 - t,        // green inverse
        0.5 + 0.5 * sin(t * 6.283), // blue oscillates
        1.0
    );
}