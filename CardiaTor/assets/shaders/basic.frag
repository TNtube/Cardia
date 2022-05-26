#version 460 core

layout(location = 0) out vec4 color;

in vec4 o_Color;
in vec2 o_TexPos;
in flat float o_TexIndex;
in float o_TilingFactor;

uniform sampler2D u_Textures[32];

void main() {
    color = texture(u_Textures[int(o_TexIndex)], o_TexPos * o_TilingFactor) * o_Color;
}