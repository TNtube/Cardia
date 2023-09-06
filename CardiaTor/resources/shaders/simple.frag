#version 450

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
    vec3 tangent;
};

layout (location = 0) in Vertex vertex;
layout (location = 4) in vec3 cameraPosition;

layout (location = 0) out vec4 fragColor;

layout(set = 1, binding = 0) uniform sampler2D texAlbedo;
layout(set = 1, binding = 1) uniform sampler2D texNormal;
layout(set = 1, binding = 2) uniform sampler2D texMetallicRoughness;
layout(set = 1, binding = 3) uniform sampler2D texAmbientOcclusion;

const vec3 lightPosition = vec3(1, 1, 1);

const float PI = 3.14159265359;

vec3 CalculateNormal(vec3 sampledNormal)
{
    if (length(vertex.tangent) == 0.0f)
    return vertex.normal;

    vec3 binormal = normalize(cross(vertex.tangent, vertex.normal));
    mat3 localAxis = mat3(binormal, vertex.tangent, vertex.normal);

    sampledNormal = (2.0f * sampledNormal) - 1.0f;

    vec3 N = normalize(vertex.normal * sampledNormal);

    N.xy *= -1.0; // x and y were flipped.
    return N;
}

float CalculateFresnel(vec3 N, vec3 V, float fPow)
{
    return pow((1 - clamp(abs(dot(N, V)), 0, 1)), fPow);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 Reinhard(vec3 v)
{
    return v / (1.0 + v);
}

void main()
{
    vec3 baseColor = texture(texAlbedo, vertex.textureCoord).rgb;
    vec3 sampledNormal = texture(texNormal, vertex.textureCoord).rgb;
    vec3 metallicRoughness = texture(texMetallicRoughness, vertex.textureCoord).rgb;
    float ao = texture(texAmbientOcclusion, vertex.textureCoord).r;

    float metallicness = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    // Alpha discard.
    float alpha = texture(texAlbedo, vertex.textureCoord).a;
    if (alpha <= 0.1f)
    discard;

    if (texture(texNormal, vertex.textureCoord).a <= 0.1f)
    discard;

    vec3 N = CalculateNormal(sampledNormal);
    vec3 V = normalize(cameraPosition - vertex.position); // View direction

    vec3 L = normalize(cameraPosition - vertex.position);
    vec3 H = normalize(V + L);

    float distance = length(cameraPosition - vertex.position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = vec3(attenuation);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallicness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallicness;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * baseColor / PI + specular) * radiance * NdotL;

    kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    kD = 1.0 - kS;
    vec3 diffuse = baseColor;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient + Lo;

    // Tonemapping
    // color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0 / 2.2));

    color = Reinhard(color);

    fragColor = vec4(color, alpha);
}