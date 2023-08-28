#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in vec3 tangent;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
    vec3 tangent;
};

layout (location = 0) out Vertex vertex;

layout(set = 0, binding = 0) uniform UBO
{
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform constants
{
    mat4 model;
    mat4 transposedInvertedModel;
} pushConstants;


void main() {
    vertex.position = (pushConstants.model * vec4(position, 1.0)).xyz;
    vertex.normal = normalize(mat3(pushConstants.transposedInvertedModel) * normal);
    vertex.textureCoord = textureCoord;
    vertex.tangent = normalize(mat3(pushConstants.transposedInvertedModel) * tangent);

    gl_Position = ubo.viewProjection * pushConstants.model * vec4(position, 1.0f);
}
