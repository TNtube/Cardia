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

const vec3 lightDirection = normalize(-vec3(-0.2f, -1.0f, -0.3f)); // direction de la lumière

void main() {
    if (o_Vertex.color.a == 0) {
        discard;
    }

    vec3 surfaceNormal = normalize(o_Vertex.normal);
    float diffuse = max(dot(surfaceNormal, lightDirection), 0.0);
    vec3 diffuseColor = vec3(1.0) * diffuse;

    vec4 color = texture(u_Texture, o_Vertex.texturePosition) * o_Vertex.color;
    OutColor = vec4(color.rgb * diffuseColor, color.a);
    OutEntityID = int(o_EntityID);
}