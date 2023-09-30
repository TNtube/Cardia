#version 450

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
    vec3 tangent;
};

layout (location = 0) in Vertex vertex;
layout (location = 4) in vec3 cameraPosition;
layout (location = 5) in float time;

layout (location = 0) out vec4 fragColor;

layout(set = 1, binding = 0) uniform sampler2D texAlbedo;
layout(set = 1, binding = 1) uniform sampler2D texNormal;
layout(set = 1, binding = 2) uniform sampler2D texMetallicRoughness;
layout(set = 1, binding = 3) uniform sampler2D texAmbientOcclusion;

const float PI = 3.14159265359;

vec3 NormalFromMap()
{
    vec3 tangentNormal = texture(texNormal, vertex.textureCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(vertex.position);
    vec3 Q2  = dFdy(vertex.position);
    vec2 st1 = dFdx(vertex.textureCoord);
    vec2 st2 = dFdy(vertex.textureCoord);

    vec3 N   = normalize(vertex.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 Reinhard(vec3 v)
{
    return v / (1.0 + v);
}

void main()
{
    vec4 alphaAlbedo = texture(texAlbedo, vertex.textureCoord);

    // Alpha discard.
    float alpha = alphaAlbedo.a;
    if (alpha <= 0.1f) discard;

    vec3 metallicRoughness = texture(texMetallicRoughness, vertex.textureCoord).rgb;

    vec3 albedo = pow(alphaAlbedo.rgb, vec3(2.2));
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;
    float ao = texture(texAmbientOcclusion, vertex.textureCoord).r; // ambient occlusion

    vec3 N = NormalFromMap(); // Normal
    vec3 V = normalize(cameraPosition - vertex.position); // View direction

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    vec3 lightPosition = vec3(20, 50, 1);
    vec3 lightColor = vec3(255.0f, 255.0f, 255.0f) * 50;

//    for (int i = 0; i < lightPositions.length(); i++) {
        // calculate per-light radiance
        vec3 L = normalize(lightPosition - vertex.position);
        vec3 H = normalize(V + L);
        float distance = length(lightPosition - vertex.position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColor * attenuation; // magic value is light color

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
//    }

    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = Reinhard(color);
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    fragColor = vec4(color, alpha);
}