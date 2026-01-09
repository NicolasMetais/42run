#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

out vec3 vColor;
out vec3 vNormal;
out vec2 TexCoord;
out vec3 FragPos; //position du vertex a envoyer dans le fragment shader

uniform mat4 MVP;
uniform mat4 model;

void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
    vColor = aColor;
    TexCoord = aTexCoord;

    FragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = mat3(transpose(inverse(model))) * aNormal;
}