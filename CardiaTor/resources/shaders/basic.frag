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


struct DirLight {
    vec4 direction;
    vec4 color;
};


layout(std430, binding = 0) buffer pointLightsBuffer
{
    PointLight pointLights[];
};

layout(std430, binding = 0) buffer dirLightsBuffer
{
    DirLight dirLights[];
};

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal)
{
    float result = dot(normal, light.direction.xyz);
    return light.color.xyz * result;
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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


void main() {
    DirLight dirLight;
    dirLight.direction = vec4(-0.5f, -0.7f, -1.0f, 1.0f);
    dirLight.color = vec4(0.8, 0.8, 0.8, 1.0f) * 0.0f;
    vec3 norm = normalize(o_Normal);
    vec3 viewDir = normalize(u_ViewPosition - o_FragPos);

    vec3 result = CalcDirLight(dirLight, norm);

    for (int i = 0; i < pointLights.length(); ++i) {
        PointLight pointLight = pointLights[i];
        result += CalcPointLight(pointLight, norm, o_FragPos, viewDir);
    }

    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * vec4(result, 1.0) * o_Color;
}
