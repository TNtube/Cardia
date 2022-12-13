#version 460 core

layout(location = 0) out vec4 color;


struct Vertex {
    vec4 color;
    vec3 normal;
    vec3 fragPosition;
    vec2 texturePosition;
    float tilingFactor;
};

in Vertex o_Vertex;
in flat float o_TexIndex;

uniform sampler2D u_Textures[32];
uniform vec3 u_ViewPosition;

void main() {
    if (o_Vertex.color.a == 0) {
        discard;
    }

    color = texture(u_Textures[int(o_TexIndex)], o_Vertex.texturePosition * o_Vertex.tilingFactor) * o_Vertex.color;
}
