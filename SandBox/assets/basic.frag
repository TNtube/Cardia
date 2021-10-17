#version 440 core

layout(location = 0) out vec4 color;

in vec2 o_TexPos;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main() {
    color = texture(u_Texture, o_TexPos) * u_Color;
}