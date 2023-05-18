#version 330 core
out vec4 FragColor;

in vec3 lighting;

uniform vec3 objectColor;

void main()
{
    FragColor = vec4(lighting * objectColor, 1.0);
}