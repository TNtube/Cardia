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
    uint lightType;
    vec4 positionAndRange;
    vec4 direction;
    vec4 color;
};


layout(std430, binding = 0) buffer LightBuffer
{
    Light lights[];
};

// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal)
{
    float result = dot(normal, light.direction.xyz);
    return light.color.xyz * result;
}


vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float dist = length(light.positionAndRange.xyz - fragPos);
    if (dist > light.positionAndRange.w) {
        return vec3(0);
    }

    vec3 lightDir = normalize(light.positionAndRange.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    // attenuation
    float attenuation = 1.0 / (1 + 0.14 * dist + 0.07 * (dist * dist));
    // combine results
    vec3 ambient = light.color.xyz;
    vec3 diffuse = light.color.xyz;
    vec3 specular = light.color.xyz;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

const uint DirectionalLight = 0x00000001u;
const uint PointLight       = 0x00000002u;
const uint SpotLight        = 0x00000004u;


void main() {
    vec3 norm = normalize(o_Normal);
    vec3 viewDir = normalize(u_ViewPosition - o_FragPos);

    vec3 result = vec3(0);

    for (int i = 0; i < lights.length(); ++i) {
        Light light = lights[i];
        if (light.lightType == DirectionalLight) {
            result += CalcDirLight(light, norm);
        }
        if (light.lightType == PointLight) {
            result += CalcPointLight(light, norm, o_FragPos, viewDir);
        }
    }

    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * vec4(result, 1.0) * o_Color;
}
