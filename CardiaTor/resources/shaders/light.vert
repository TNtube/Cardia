#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexPos;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in float a_EntityID;


struct Vertex {
    vec4 color;
    vec3 normal;
    vec3 fragPosition;
    vec2 texturePosition;
    float tilingFactor;
};

layout (location = 0) out Vertex o_Vertex;
layout (location = 5) out flat float o_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat3 u_TransposedInvertedModel;

void main() {
    o_Vertex.fragPosition = vec3(u_Model * vec4(a_Position, 1.0f));
    o_Vertex.normal = u_TransposedInvertedModel * a_Normal;
    o_Vertex.color = a_Color;
    o_Vertex.texturePosition = a_TexPos;
    o_Vertex.tilingFactor = a_TilingFactor;
    o_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);
}