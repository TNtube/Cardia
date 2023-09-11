#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 0) out vec4 fragColor;

layout(set = 1, binding = 0) uniform samplerCube cubemap;

void main()
{
    vec3 color = texture(cubemap, vPosition).rgb;

    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    fragColor = vec4(color, 1.0);
}
