
const uint LIGHT_POINT       = 0;
const uint LIGHT_DIRECTIONAL = 1;
const uint LIGHT_SPOT        = 2;
struct Light
{
    uint type;
    vec4 color;
    float intensity;

    uint _padding0[3];

    vec4 position;
    vec4 direction;

    float range;
    float spotInnerAngle;
    float spotOuterAngle;

    uint castsShadow;
    uint shadowMapIndex;

    mat4 viewProj;

    uint _padding1[2];
};

vec3 GetRadiancePoint(Light light, float dist)
{
    float rangeFactor = clamp(1.0 - dist / light.range, 0.0, 1.0);
    float attenuation = rangeFactor*rangeFactor / max(dist * dist, 0.0001);
    return light.color.rgb * light.intensity * attenuation;
}
vec3 GetRadianceDir(Light light)
{
    return light.color.rgb * light.intensity;
}
vec3 GetRadianceSpot(Light light, float dist, vec3 lightToFragment)
{
    float rangeFactor = clamp(1.0 - dist / light.range, 0.0, 1.0);
    float attenuation = rangeFactor*rangeFactor / max(dist * dist, 0.0001);

    float theta = dot(lightToFragment, normalize(light.direction.xyz));
    float inner = cos(light.spotInnerAngle);
    float outer = cos(light.spotOuterAngle);

    float intensity = smoothstep(outer, inner, theta);

    return light.color.rgb * light.intensity * attenuation * intensity;
}

