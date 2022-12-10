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


struct Light {
    vec4 positionAndType;
    vec4 directionAndRange;
    vec4 colorAndCutOff;
};


layout(std430, binding = 0) buffer LightBuffer
{
    Light lights[];
};

// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal)
{
    float diff = max(dot(normal, light.directionAndRange.xyz), 0.0);
    return light.colorAndCutOff.xyz * diff * o_Color.rgb;
}


vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float range = light.directionAndRange.w;
    float dist = length(light.positionAndType.xyz - fragPos);
    // attenuation
    float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist) * max(smoothstep(1.0, 0.0, dist / range), 0.0);
    return light.colorAndCutOff.xyz * o_Color.rbg * attenuation;
}


vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float range = light.directionAndRange.w;
    float dist = length(light.positionAndType.xyz - fragPos);

    float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist) * max(smoothstep(1.0, 0.0, dist / range), 0.0);

    vec3 lightDir = normalize(light.positionAndType.xyz - fragPos);

    float theta = dot(lightDir, normalize(-light.directionAndRange.xyz));
    float epsilon = light.colorAndCutOff.w - (light.colorAndCutOff.w - 0.09);
    float intensity = clamp((theta - light.colorAndCutOff.w - 0.09) / epsilon, 0.0, 1.0);

    return light.colorAndCutOff.xyz * o_Color.rgb * attenuation * intensity;
}

const uint DirectionalLight = 0x00000000u;
const uint PointLight       = 0x00000001u;
const uint SpotLight        = 0x00000002u;


void main() {
    if (o_Color.a == 0) {
        discard;
    }
    vec3 norm = normalize(o_Normal);
    vec3 viewDir = normalize(u_ViewPosition - o_FragPos);

    vec3 result = vec3(0);

    for (int i = 0; i < lights.length(); ++i) {
        Light light = lights[i];
        if (light.positionAndType.w == DirectionalLight) {
            result += CalcDirLight(light, norm);
        }
        if (light.positionAndType.w == PointLight) {
            result += CalcPointLight(light, norm, o_FragPos, viewDir);
        }
        if (light.positionAndType.w == SpotLight) {
            result += CalcSpotLight(light, norm, o_FragPos, viewDir);
        }
    }

    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * vec4(result, 1.0);
}
