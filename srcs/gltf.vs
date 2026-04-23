#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoord0;
layout(location = 4) in vec2 aTexCoord1;
layout(location = 5) in vec4 aColor0;
layout(location = 6) in vec4 aColor1;
layout(location = 7) in vec4 aJoints;
layout(location = 8) in vec4 aWeights;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 VertexColor;
out mat3 TBN;

uniform mat4 MVP;
uniform mat4 model;
uniform bool hasTangent;
uniform bool hasVertexColor;
uniform bool hasSkin;
uniform mat4 jointMatrices[64];

void main() {
    vec4 pos;
    vec3 norm;

    if (hasSkin) {
        pos  = aWeights.x * jointMatrices[int(aJoints.x)] * vec4(aPos, 1.0)
             + aWeights.y * jointMatrices[int(aJoints.y)] * vec4(aPos, 1.0)
             + aWeights.z * jointMatrices[int(aJoints.z)] * vec4(aPos, 1.0)
             + aWeights.w * jointMatrices[int(aJoints.w)] * vec4(aPos, 1.0);
        norm = aWeights.x * mat3(jointMatrices[int(aJoints.x)]) * aNormal
             + aWeights.y * mat3(jointMatrices[int(aJoints.y)]) * aNormal
             + aWeights.z * mat3(jointMatrices[int(aJoints.z)]) * aNormal
             + aWeights.w * mat3(jointMatrices[int(aJoints.w)]) * aNormal;
        norm = normalize(norm);
    } else {
        pos  = vec4(aPos, 1.0);
        norm = aNormal;
    }

    gl_Position = MVP * pos;
    TexCoord = aTexCoord0;
    FragPos = vec3(model * pos);

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 N = normalize(normalMatrix * norm);
    Normal = N;
    
    if (hasVertexColor)
        VertexColor = aColor0;
    else
        VertexColor = vec4(1.0);

    if (hasTangent) {
        vec3 T = normalize(normalMatrix * aTangent.xyz);
        T = normalize(T - N * dot(N, T));
        vec3 B = normalize(cross(N, T)) * aTangent.w;
        TBN = mat3(T, B, N);
    }
    else {
        // No tangent data: build a fallback TBN so tbn[2] = actual surface normal
        vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        vec3 T2 = normalize(cross(up, N));
        vec3 B2 = cross(N, T2);
        TBN = mat3(T2, B2, N);
    }
}