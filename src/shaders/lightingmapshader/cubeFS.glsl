#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse; // as ambient is always the same as diffuse
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    vec3 textureColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularRatio = vec3(texture(material.specular, texCoords));

    vec3 ambient = textureColor * light.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = textureColor * cosine * light.diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularRatio * spec * light.specular;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}