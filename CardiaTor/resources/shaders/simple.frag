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

const float PI = 3.14159265359;

const vec3 lightDirection = -vec3(0.577, -0.577, -0.577);

void main() {
    vec4 baseColor = texture(texAlbedo, vertex.textureCoord);

    vec3 normal = normalize(vertex.normal);

    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    diffuseStrength = diffuseStrength * 0.5 + 0.5;
    diffuseStrength = clamp(diffuseStrength, 0.0, 1.0);

    vec4 finalColor = baseColor * diffuseStrength;


    if (finalColor.a <= 0.01)
        discard;
    fragColor = finalColor;
}
