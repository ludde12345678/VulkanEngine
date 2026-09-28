#include "./Math.glsl"

struct Material
{
 vec4 baseColor;
 vec4 specularColor;

 float specularPower;
 float Metallic;
 float roughness;
 float ambientOcclusion;

 uint AlbedoTexture;
 uint NormalTexture;
 uint RoughnessTexture;
 uint MetallicTexture;
 uint AOTexture;

 uint _padding[3];
};

vec3 GetF0(vec3 albedo, float metallic)
{
    return mix(vec3(0.04), albedo, metallic);
}

// Cook torrence BRDF: D term
float D_GGX(float alpha, vec3 N, vec3 H){
    float aSquared = alpha*alpha;

    float dotNH = max(dot(N, H), 0.0);
    float dotNHSquared = dotNH*dotNH;
    float denom = (dotNHSquared*(aSquared-1.0)+1.0);
    float denomFinal =  PI * denom*denom;

    return (aSquared/denomFinal);
}
// Cook torrence BRDF: F term
vec3 F_Schlick(vec3 F0, vec3 V, vec3 H){
    float VdotH = max(dot(V, H), 0.0);
    float oneMinusVdotH = 1.0 - VdotH;
    float VdotHPower = oneMinusVdotH*oneMinusVdotH*oneMinusVdotH*oneMinusVdotH*oneMinusVdotH;
    return F0 + (1.0-F0) * VdotHPower;
}

// Cook torrence BRDF: G term
float G_Smith(vec3 V, vec3 L, vec3 N, float alpha){
    float aSquared = alpha*alpha;
    float VdotN = max(dot(V, N),  0.001);
    float VdotNSquared = pow(VdotN,2.0);
    float tanSquared = (1.0-VdotNSquared)/(VdotNSquared);
    float sqrtvalue = sqrt(1.0+aSquared*tanSquared);
    float G_V = (2.0/(1.0+sqrtvalue));

    float LdotN = max(dot(L, N),  0.001);
    float LdotNSquared = pow(LdotN,2.0);
    float tanSquared2 = (1.0-LdotNSquared)/(LdotNSquared);
    float sqrtvalue2 = sqrt(1.0+aSquared*tanSquared2);
    float G_L = (2.0/(1.0+sqrtvalue2));

    return G_V * G_L;
}

// Cook torrence BRDF
vec3 CookTorrence(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0){
	vec3 H = normalize(V + L);

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float alpha = pow(roughness, 2.0);

    float D = D_GGX(alpha, N, H);
    vec3  F = F_Schlick(F0, V, H);
    float G = G_Smith(V, L, N, alpha);

    return (D * F * G) /
           max(4.0 * NdotV * NdotL, 0.001);
}