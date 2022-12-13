#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexPos;
layout(location = 4) in float a_TexIndex;
layout(location = 5) in float a_TilingFactor;


struct Vertex {
    vec4 color;
    vec3 normal;
    vec3 fragPosition;
    vec2 texturePosition;
    float tilingFactor;
};

out Vertex o_Vertex;
out flat float o_TexIndex;

uniform mat4 u_ViewProjection;

void main() {
    o_Vertex.color = a_Color;
    o_Vertex.normal = a_Normal;
    o_Vertex.fragPosition = a_Position;
    o_Vertex.texturePosition = a_TexPos;
    o_Vertex.tilingFactor = a_TilingFactor;
    o_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}