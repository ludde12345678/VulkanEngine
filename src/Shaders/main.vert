#version 460



layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outnormal;
layout(location = 2) out vec2 outtexcoord;


layout(set = 0, binding = 0) uniform TimeUBO
{
    uint FrameCount;
} Time;

layout(set = 0, binding = 1) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
} Camera;

layout(push_constant) uniform PushConsts
{
    mat4 modelMatrix;
} pushConsts;

void main()
{

    vec4 worldPos = pushConsts.modelMatrix * vec4(pos,1.0);

    outnormal = mat3(transpose(inverse(pushConsts.modelMatrix))) * normal;
    outWorldPos = worldPos.xyz;
    outtexcoord = texcoord;

    gl_Position = Camera.projection*Camera.view*worldPos;

}