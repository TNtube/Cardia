#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 0) in vec2 a_TexPos;

out vec4 o_Color;
out vec2 o_TexPos;

uniform mat4 u_ViewProjection;

void main() {
    o_Color = a_Color;
    o_TexPos = a_TexPos;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}