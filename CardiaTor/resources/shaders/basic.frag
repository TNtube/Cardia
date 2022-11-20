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
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

layout(std430, binding = 0) buffer lightBuffer
{
    Light lights[];
};


vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // attenuation
    float distance    = length(light.position.xyz - fragPos);
    // combine results
    vec3 ambient  = light.ambient.xyz * 0;
    vec3 diffuse  = light.diffuse.xyz  * diff;
    vec3 specular = light.specular.xyz * spec;
    return (ambient + diffuse + specular);
}

void main() {
    vec3 norm = normalize(o_Normal);
    vec3 viewDir = normalize(u_ViewPosition - o_FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < lights.length(); ++i)
    {
        Light currentLight = lights[i];
        result += CalcLight(currentLight, norm, o_FragPos, viewDir);
    }
//    Light currentLight = lights[0];
//    result += CalcLight(currentLight, norm, o_FragPos, viewDir);

    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * vec4(result, 1.0) * o_Color;
}
