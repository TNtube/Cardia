#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 textureCoord;

struct Vertex {
    vec3 color;
    vec3 normal;
    vec2 textureCoord;
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
    gl_Position = ubo.viewProjection * pushConstants.model * vec4(position, 1.0f);
    vertex.color = color;
    vertex.normal = mat3(pushConstants.transposedInvertedModel) * normal;
    vertex.textureCoord = textureCoord;
}
