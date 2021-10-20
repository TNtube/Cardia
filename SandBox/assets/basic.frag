#version 440 core

layout(location = 0) out vec4 color;

in vec4 o_Color;
in vec2 o_TexPos;

uniform sampler2D u_Texture;

void main() {
    color = texture(u_Texture, o_TexPos) * o_Color;
}