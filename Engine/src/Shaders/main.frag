#version 460

#include "./common/Light.glsl"
#include "./common/Material.glsl"
#include "./common/Config.glsl"
layout(location = 0) in vec3 WorldPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec4 outColor;

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

layout(std430,set = 0, binding = 2) readonly buffer LightBuffer
{
    Light lights[];
}; 
layout(set = 1, binding = 0) readonly buffer MaterialBuffer
{
    Material materials[];
};  

layout(set=1, binding=1) uniform sampler2D textures[MAX_TEXTURES];
layout(set=2, binding=1) uniform sampler2DShadow shadowMap[MAX_SHADOW_MAPS];
layout(push_constant) uniform PushConsts
{
    mat4 modelMatrix;
    uint materialIndex;
    
} pushConsts;

// shadowMap Func
float GetShadow(Light light)
{
     vec4 shadowPos =  light.viewProj * vec4(WorldPos, 1.0);
     vec3 shadowCoord = shadowPos.xyz / shadowPos.w;
     shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
     if (shadowCoord.x < 0.0 || shadowCoord.x > 1.0 ||
        shadowCoord.y < 0.0 || shadowCoord.y > 1.0 ||
        shadowCoord.z < 0.0 || shadowCoord.z > 1.0)
     {
        return 1.0;
     }
     float bias = 0.00005;
     float shadow = 0.0;
     int kernelSize = 1;
     float texelSize = 1.0 / float(16384);
     if(kernelSize == 0){
     return texture( shadowMap[light.shadowMapIndex],vec3(shadowCoord.xy + 0, shadowCoord.z - bias));
     }
     for(int x = 0-kernelSize; x < kernelSize; x++){
        for(int y = 0-kernelSize; y < kernelSize; y++){
            vec2 offset = vec2(x, y) * texelSize;
             shadow += texture( shadowMap[light.shadowMapIndex],vec3(shadowCoord.xy + offset, shadowCoord.z - bias));
        }
     }
     shadow /= (kernelSize*2+1) * (kernelSize*2+1);
     return shadow;
}

void main()
{	
    // Indexes
    Material material = materials[pushConsts.materialIndex];
    uint  albedoIndex = material.AlbedoTexture;
    uint  normalIndex = material.NormalTexture;
    uint  roughnessIndex = material.RoughnessTexture;
    uint  metallicIndex = material.MetallicTexture;
    uint  AOIndex = material.AOTexture;
    // textures
    vec3 albedo = texture(textures[albedoIndex], texcoord).xyz * vec3(material.baseColor.xyz);
    float roughness = texture(textures[roughnessIndex], texcoord).r *material.roughness;
    float metallic = texture(textures[metallicIndex], texcoord).r *material.Metallic;
    float ao = texture(textures[AOIndex], texcoord).r * material.ambientOcclusion;
    // Normal Handling
    vec3 normalTexture =texture(textures[normalIndex], texcoord).xyz;

    // Vectors
    vec3 N = normalize(normal);
    vec3 V = normalize(Camera.position.xyz - WorldPos);
    // dot products

    float NdotV = max(dot(N, V), 0);

    // F0
    vec3 F0 = GetF0(albedo, metallic);

    vec3 L0 = vec3(0.0);
    for(uint i=0; i< Frame.lightCount; i++){
        // for every light
        vec3 L;
        vec3 radience;
        if(lights[i].type == LIGHT_POINT){
            vec3 toLight = lights[i].position.xyz - WorldPos;
            float dist = length(toLight);
            L = toLight / dist;
            radience = GetRadiancePoint(lights[i], dist);
            
        }
        else if(lights[i].type == LIGHT_DIRECTIONAL){
            L = normalize(-lights[i].direction.xyz);
            radience = GetRadianceDir(lights[i]);
            if(lights[i].castsShadow == 1){
                radience = radience *GetShadow(lights[i]);
            }
        }
        else if(lights[i].type == LIGHT_SPOT){
            vec3 toLight = lights[i].position.xyz - WorldPos;
            float dist = length(toLight);
            L = toLight / dist;

            vec3 lightToFragment = normalize(WorldPos - lights[i].position.xyz);
            radience = GetRadianceSpot(lights[i], dist, lightToFragment);
        }
        else{
            L = normalize(lights[i].position.xyz - WorldPos);
            radience = lights[i].color.rgb * lights[i].intensity;
        }
        // specular PBR
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0);

        vec3 specular = CookTorrence(N, V, L, roughness, F0);
        // metallic
        vec3 F = F_Schlick(F0, V, H);
        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

        vec3 diffuse = kD * albedo / PI;
        vec3 brdf = diffuse + specular;
        L0 += brdf * radience * NdotL;
    }
    outColor = vec4(L0, 1);
}


