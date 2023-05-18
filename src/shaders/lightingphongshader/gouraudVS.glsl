#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 lighting;

uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform float specularStrength;
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));

    // nomarl matirx, actually 'inverse' computaion is pretty slow on GPU 
    vec3 normal = mat3(transpose(inverse(model))) * aNormal;
    
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = cosine * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    lighting = ambient + diffuse + specular;
}