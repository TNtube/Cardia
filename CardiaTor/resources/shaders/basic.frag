#version 460 core

layout(location = 0) out vec4 color;

in vec4 o_Color;
in vec3 o_Normal;
in vec3 o_FragPos;
in vec2 o_TexPos;
in flat float o_TexIndex;
in float o_TilingFactor;

uniform sampler2D u_Textures[32];
uniform vec3 u_ViewPosition;


struct PointLight {
    vec4 positionAndRange;
    vec4 color;
};


layout(std430, binding = 0) buffer lightBuffer
{
    PointLight pointLights[];
};


vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 viewDir)
{
    // calculate normalized light vector and distance to sphere light surface
    float range = light.positionAndRange.w;
    float dist = distance(light.positionAndRange.xyz, fragPos);

    float attenuation = 1 - dist / range;

    attenuation = max(attenuation, 0.0f);

    return light.color.xyz * attenuation;
}

void main() {
    vec3 norm = normalize(o_Normal);
    vec3 viewDir = normalize(u_ViewPosition - o_FragPos);

    vec3 result = vec3(0);
    for (int i = 0; i < pointLights.length(); ++i) {
        PointLight pointLight = pointLights[i];
        result += CalcPointLight(pointLight, o_FragPos, viewDir);
    }

    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * vec4(result, 1.0) * o_Color;
}
