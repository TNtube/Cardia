#version 460 core

layout(location = 0) out vec4 OutColor;
layout(location = 1) out int OutEntityID;


struct Vertex {
    vec4 color;
    vec3 normal;
    vec3 fragPosition;
    vec2 texturePosition;
    float tilingFactor;
};

layout (location = 0) in Vertex o_Vertex;
layout (location = 5) in flat float o_EntityID;

uniform sampler2D u_Texture;
uniform vec3 u_ViewPosition;

void main() {
    if (o_Vertex.color.a == 0) {
        discard;
    }

    OutColor = texture(u_Texture, o_Vertex.texturePosition);
    OutEntityID = int(o_EntityID);
}
