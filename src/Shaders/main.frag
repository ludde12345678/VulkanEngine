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

	int debugNormals;
} Shading;


layout(location = 0) out vec4 outColor;

void main()
{	
	// vectors
    vec3 L = normalize(Shading.lightpos.xyz - WorldPos);
	vec3 V = normalize(Shading.camerapos.xyz - WorldPos);
	vec3 H = normalize(L + V);

	//ambient
	vec3 amb;
	if(Shading.toggleAmbient != 0){
		amb = Shading.ambientColor.xyz;
	}
	else{amb = vec3(0);}

	// diffuse

	vec3 diff;
	float dotLN =  max(dot(normalize(normal),L), 0.0);
	if(Shading.toggleDiffuse != 0){
		diff = Shading.diffuseColor.xyz * dotLN;
	}
	else{diff = vec3(0);}

	//spectral
	float dotHN =  max(dot(normalize(normal), H), 0.0);
	vec3 spec;
	if(Shading.toggleSpecular != 0){
		spec = Shading.spectralColor.xyz*pow(dotHN, Shading.spectralPower);
	}
	else{spec = vec3(0);}

	if(Shading.debugNormals != 1){
		outColor =  vec4(amb + diff + spec, 1.0);
	}
	else{
		outColor = vec4(normalize(normal) * 0.5 + 0.5, 1.0);
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