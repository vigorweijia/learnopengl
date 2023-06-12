#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec3 position;
} gs_in[];

out vec2 TexCoords; 

uniform float time;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec4 explode(vec3 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return vec4(position, 1.0) + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gs_in[0].position) - vec3(gs_in[1].position);
    vec3 b = vec3(gs_in[2].position) - vec3(gs_in[1].position);
    return normalize(cross(a, b));
}

void main() {    
    mat4 mvp = projection * view * model;
    vec3 normal = GetNormal();

    gl_Position = mvp * explode(gs_in[0].position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();

    gl_Position = mvp * explode(gs_in[1].position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();

    gl_Position = mvp * explode(gs_in[2].position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();

    EndPrimitive();
}