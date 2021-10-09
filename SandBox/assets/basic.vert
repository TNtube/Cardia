#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexPos;

out vec2 o_TexPos;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main() {
    o_TexPos = a_TexPos;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);
}