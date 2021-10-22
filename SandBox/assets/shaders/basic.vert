#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexPos;
layout(location = 3) in int a_TexIndex;

out vec4 o_Color;
out vec2 o_TexPos;
out flat int o_TexIndex;

uniform mat4 u_ViewProjection;

void main() {
    o_Color = a_Color;
    o_TexPos = a_TexPos;
    o_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}