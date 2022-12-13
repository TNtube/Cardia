#version 460 core

layout(location = 0) out vec4 color;


struct Vertex {
    vec4 color;
    vec3 normal;
    vec3 fragPosition;
    vec2 texturePosition;
    float tilingFactor;
};

struct Light {
    vec4 positionAndType;
    vec4 directionAndRange;
    vec4 colorAndCutOff;
};


in Vertex o_Vertex;
in flat float o_TexIndex;

uniform sampler2D u_Textures[32];
uniform vec3 u_ViewPosition;


layout(std430, binding = 0) buffer LightBuffer
{
    Light lights[];
};


vec3 CalcDirLight(Light light, vec3 normal)
{
    float diff = max(dot(normal, light.directionAndRange.xyz), 0.0);
    return light.colorAndCutOff.xyz * diff * o_Vertex.color.rgb;
}


vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float range = light.directionAndRange.w;
    float dist = length(light.positionAndType.xyz - fragPos);
    float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist) * max(smoothstep(1.0, 0.0, dist / range), 0.0);

    return light.colorAndCutOff.xyz * o_Vertex.color.rbg * attenuation;
}


vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float range = light.directionAndRange.w;
    float dist = length(light.positionAndType.xyz - fragPos);
    vec3 lightDir = normalize(light.positionAndType.xyz - fragPos);

    float attenuation = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist) * max(smoothstep(1.0, 0.0, dist / range), 0.0);

    float theta = dot(lightDir, normalize(-light.directionAndRange.xyz));
    float epsilon = light.colorAndCutOff.w - (light.colorAndCutOff.w - 0.09);
    float intensity = clamp((theta - light.colorAndCutOff.w - 0.09) / epsilon, 0.0, 1.0);

    return light.colorAndCutOff.xyz * o_Vertex.color.rgb * attenuation * intensity;
}

const uint DirectionalLight = 0x00000000u;
const uint PointLight       = 0x00000001u;
const uint SpotLight        = 0x00000002u;


void main() {
    if (o_Vertex.color.a == 0) {
        discard;
    }
    vec3 norm = normalize(o_Vertex.normal);
    vec3 viewDir = normalize(u_ViewPosition - o_Vertex.fragPosition);

    vec3 result = vec3(0);

    for (int i = 0; i < lights.length(); ++i) {
        Light light = lights[i];
        if (light.positionAndType.w == DirectionalLight) {
            result += CalcDirLight(light, norm);
        }
        if (light.positionAndType.w == PointLight) {
            result += CalcPointLight(light, norm, o_Vertex.fragPosition, viewDir);
        }
        if (light.positionAndType.w == SpotLight) {
            result += CalcSpotLight(light, norm, o_Vertex.fragPosition, viewDir);
        }
    }

    color = texture(u_Textures[int(o_TexIndex)], o_Vertex.texturePosition * o_Vertex.tilingFactor) * vec4(result, 1.0);
}
