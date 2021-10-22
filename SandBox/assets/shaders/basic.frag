#version 440 core

layout(location = 0) out vec4 color;

in vec4 o_Color;
in vec2 o_TexPos;
in flat int o_TexIndex;

uniform sampler2D[32] u_Textures;

void main() {
    color = texture(u_Textures[o_TexIndex], o_TexPos) * o_Color;
}