#version 460
#include "./common/Light.glsl"
#include "./common/Material.glsl"
#include "./common/Config.glsl"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(set = 0, binding = 0) uniform FrameUBO
{
	uint FrameCount;
	float deltaTime;
	float totalTime;
	uint lightCount;
} Frame;

layout(set = 0, binding = 1) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 position;
} Camera;
layout(set = 0, binding = 2) readonly buffer LightBuffer
{
    Light lights[];
};  
layout(set = 1, binding = 0) readonly buffer MaterialBuffer
{
    Material material[];
};  
layout(push_constant) uniform PushConsts
{
    mat4 modelMatrix;
    uint materialIndex;
    uint shadowLightIndex;
    
} pushConsts;

uint SM_findLightFromCount(uint shadowLightIndex){
    uint foundCount = 0;
    for (uint i = 0; i < Frame.lightCount; i++){
        if (lights[i].castsShadow == 1){
            
            if (foundCount == shadowLightIndex){return i;}
            foundCount++;
        }
    }
    return Frame.lightCount;
}


void main()
{
    vec4 worldPos = pushConsts.modelMatrix * vec4(pos, 1.0);

    uint lightIndex = SM_findLightFromCount(pushConsts.shadowLightIndex);

    gl_Position = lights[lightIndex].viewProj * worldPos;

}




