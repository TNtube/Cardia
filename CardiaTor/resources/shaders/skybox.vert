#version 450

layout(location = 0) in vec3 position;

layout (location = 0) out vec3 vPosition;

layout(set = 0, binding = 0) uniform UBO
{
    mat4 proj;
    mat4 model;
} ubo;

void main() {
    vPosition = position;
//    vPosition.x *= -1.0;
    gl_Position = ubo.proj * ubo.model * vec4(position.xyz, 1.0);
}
