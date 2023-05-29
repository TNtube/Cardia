#version 450

struct Vertex {
    vec3 color;
    vec3 normal;
    vec2 textureCoord;
};
layout (location = 0) in Vertex vertex;

layout (location = 0) out vec4 outColor;

const vec3 lightDirection = normalize(-vec3(-0.2f, -1.0f, -0.3f));

//layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main() {
    vec3 surfaceNormal = normalize(vertex.normal);
    float diffuse = max(dot(surfaceNormal, lightDirection), 0.0);

    vec4 color = /**texture(texSampler, vertex.textureCoord) **/ vec4(vertex.color, 1.0f);
    outColor = color * diffuse;
}
