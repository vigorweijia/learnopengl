#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec3 viewLightPos;

uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(viewLightPos - fragPos);
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = cosine * lightColor;

    vec3 viewDir = normalize(-fragPos); // in viewspace, camera is always in (0, 0, 0), viewPos - fragPos = -fragPos
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}