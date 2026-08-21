#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform bool debugMode;

void main()
{
    if (debugMode) { FragColor = vec4(0.4275, 0.4275, 0.4275, 0.938); return; }
    vec3 color = texture(skybox, TexCoords).rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}
