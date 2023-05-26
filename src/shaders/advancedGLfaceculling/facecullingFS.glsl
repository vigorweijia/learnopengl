#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

float near = 0.1;
float far = 100.0;

uniform sampler2D texture1;

void main()
{    
    FragColor = texture(texture1, TexCoords);
    // float depth = gl_FragCoord.z;
    // float z = depth * 2.0 - 1.0; // back to NDC 
    // float linear_z =  (2.0 * near * far) / (far + near - z * (far - near));
    // FragColor = vec4(vec3(depth), 1.0);
}