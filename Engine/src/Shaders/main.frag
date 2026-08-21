#version 460

layout(location = 0) in vec3 WorldPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;





layout(set = 0, binding = 2) uniform ShadingUBO
{
	vec4 diffuseColor;
	vec4 spectralColor;
	vec4 ambientColor;

	vec4 lightpos;
	vec4 camerapos;

	float spectralPower;

	// toggles
	int toggleDiffuse;
	int toggleSpecular;
	int toggleAmbient;

	int toggleAlbedo;
	int toggleNormal;
	int toggleRoughness;

	int debugNormals;
} Shading;

layout(set=0, binding=3) uniform sampler2D textures[100];
layout(push_constant) uniform PushConsts
{
    mat4 modelMatrix;

	uint AlbedoIndex;
	uint NormalIndex;
	uint RoughnessIndex;
	vec4 color;
	float specularIntensity;
} pushConsts;

layout(location = 0) out vec4 outColor;

void main()
{	
	// vectors
    vec3 L = normalize(Shading.lightpos.xyz - WorldPos);
	vec3 V = normalize(Shading.camerapos.xyz - WorldPos);
	vec3 H = normalize(L + V);

	vec3 N0 = normalize(normal);

vec3 dp1 = dFdx(WorldPos);
vec3 dp2 = dFdy(WorldPos);

vec2 duv1 = dFdx(texcoord);
vec2 duv2 = dFdy(texcoord);

vec3 T = normalize(dp1 * duv2.y - dp2 * duv1.y);
T = normalize(T - N0 * dot(N0, T));

vec3 B = cross(N0, T);

mat3 TBN = mat3(T, B, N0);

// Albedo
vec3 albedo;
if (Shading.toggleAlbedo != 0)
{
    albedo = texture(
        textures[pushConsts.AlbedoIndex],
        texcoord
    ).rgb;
}
else
{
    albedo = pushConsts.color.rgb;
}


// Normal
vec3 N;
if (Shading.toggleNormal != 0)
{
    vec3 normalMap = texture(
        textures[pushConsts.NormalIndex],
        texcoord
    ).xyz;

    normalMap = normalMap * 2.0 - 1.0;
    N = normalize(TBN * normalMap);
}
else
{
    N = normalize(normal);
}


// Roughness
float roughness;
if (Shading.toggleRoughness != 0)
{
    roughness = texture(
        textures[pushConsts.RoughnessIndex],
        texcoord
    ).r;
}
else
{
    roughness = 0.5;
}

float shininess = mix(128.0, 2.0, roughness);

	//ambient
	vec3 amb;
	if(Shading.toggleAmbient != 0){
		amb = Shading.ambientColor.xyz;
	}
	else{amb = vec3(0);}

	// diffuse

	vec3 diff;
	float dotLN =  max(dot(normalize(N),L), 0.0);
	if(Shading.toggleDiffuse != 0){
		diff = albedo * dotLN;
	}
	else{diff = vec3(0);}

	//spectral
	float dotHN =  max(dot(normalize(N), H), 0.0);
	vec3 spec;
	if(Shading.toggleSpecular != 0){
		spec = Shading.spectralColor.xyz*pow(dotHN, shininess);
	}
	else{spec = vec3(0);}

	if(Shading.debugNormals != 1){
		outColor =  vec4(amb + diff + spec, 1.0);
	}
	else{
		outColor = vec4(normalize(N) * 0.5 + 0.5, 1.0);
	}
// visualise back/front faces
if(false){
if (gl_FrontFacing)
{
    outColor = vec4(0,1,0,1);
}
else
{
    outColor = vec4(1,0,0,1);
}
}
}