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

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
    vec3 textureColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularRatio = vec3(texture(material.specular, texCoords));

    vec3 ambient = textureColor * light.ambient;

    vec3 lightDir = normalize(-light.direction); // light direction is known
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = textureColor * cosine * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularRatio * spec * light.specular;

    return ambient + diffuse + specular;
}

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 textureColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularRatio = vec3(texture(material.specular, texCoords));

    vec3 ambient = textureColor * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = textureColor * cosine * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularRatio * spec * light.specular;

    return (ambient + diffuse + specular) * attenuation;
}

struct SpotLight {
    vec3 position; // for spotlight
    vec3 direction; // for spotlight
    float cutoff; // for spotlight, cosine \phi
    float outerCutoff; // make light edge more smooth
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant; // for point light source
    float linear; // for point light source
    float quadratic; // for point light source
};
uniform SpotLight spotLight;

vec3 CalcSpotlight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 textureColor = vec3(texture(material.diffuse, texCoords));
    vec3 specularRatio = vec3(texture(material.specular, texCoords));

    vec3 ambient = textureColor * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float cosine = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = textureColor * cosine * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularRatio * spec * light.specular;

    float cosTheta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((cosTheta - light.outerCutoff)/epsilon, 0.0, 1.0);
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    
    // Directional Light
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // Point Lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    
    // Spotlight
    result += CalcSpotlight(spotLight, norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}