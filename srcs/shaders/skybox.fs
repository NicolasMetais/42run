#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform bool debugMode;

void main()
{
    if (debugMode) { FragColor = vec4(0.4275, 0.4275, 0.4275, 0.938); return; }
    FragColor = texture(skybox, TexCoords);
}
