#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoord0;
layout(location = 4) in vec2 aTexCoord1;
layout(location = 5) in vec4 aColor0; //vec4, si pas d'alpha openGL mettra 1.0 alpha tout seul
layout(location = 6) in vec4 aColor1;


out vec2 TexCoord;
out vec3 FragPos; //position du vertex a envoyer dans le fragment shader
out vec3 Normal;
out vec4 VertexColor;
out mat3 TBN;

uniform mat4 MVP;
uniform mat4 model;
uniform bool hasTangent;
uniform bool hasVertexColor;

void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord0;
    FragPos = vec3(model * vec4(aPos, 1.0));

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 N = normalize(normalMatrix * aNormal);
    Normal = N;
    
    if (hasVertexColor)
        VertexColor = aColor0;
    else
        VertexColor = vec4(1.0);

    if (hasTangent) {
        vec3 T = normalize(normalMatrix * aTangent.xyz);
        T = normalize(T - N * dot(N, T));
        vec3 B = normalize(cross(T, N)) * aTangent.w;
        TBN = mat3(T,B,N);
    }
    else
        TBN = mat3(
            1,0,0,
            0,1,0,
            0,0,1
        );
}